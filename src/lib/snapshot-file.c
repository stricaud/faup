#define _GNU_SOURCE // For asprintf()
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include <faup/faup.h>
#include <faup/snapshot.h>


static int _read_item(faup_snapshot_t *snapshot, FILE *fp, char *item_name)
{
  size_t value_len;
  size_t readsize;
  faup_snapshot_item_t *item;
  faup_snapshot_value_count_t *vc;
  
  while (!feof(fp)) {
    vc = faup_snapshot_value_count_new();
    readsize = fread(&value_len, sizeof(size_t), 1, fp);
    if (readsize <= 0) {
      faup_snapshot_value_count_free(vc);
      break;
    }
    vc->value = malloc(value_len + 1);
    readsize = fread(vc->value, value_len, 1, fp);
    vc->value[value_len] = '\0';
    readsize = fread(&vc->count, sizeof(size_t), 1, fp);
    readsize = fread(&vc->first_time_seen, sizeof(time_t), 1, fp);
    readsize = fread(&vc->last_time_seen, sizeof(time_t), 1, fp);
    /* faup_snapshot_value_count_debug(vc); */
    /* faup_snapshot_value_count_free(vc); */

    faup_snapshot_item_append(snapshot, item_name);
    item = faup_snapshot_item_get(snapshot, item_name);
    faup_snapshot_value_count_append_object(item, vc);
  }
  
  
  return 0;
}

/* Read a faup_snapshot directory to build the faup_snapshot_t * structure */
faup_snapshot_t *faup_snapshot_read(char *dirpath)
{
  faup_snapshot_t *snapshot;
  DIR *dir;
  struct dirent *ent;
  FILE *fp;
  
  dir = opendir(dirpath);
  if (!dir) {
    fprintf(stderr, "Error reading directory '%s': %s\n", dirpath, strerror(errno));
    return NULL;
  }
  snapshot = faup_snapshot_open(dirpath);
  while ((ent = readdir(dir)) != NULL) {
    if ((ent->d_name) && (ent->d_name[0] != '.') && (ent->d_name[0] != '_')) {
      char *full_file_path;
      int retval;
      retval = asprintf(&full_file_path, "%s%c%s", dirpath, FAUP_OS_DIRSEP_C, ent->d_name);
      fp = fopen(full_file_path, "rb");
      fseek(fp, 0, SEEK_SET);
      
      _read_item(snapshot, fp, ent->d_name);
      
      fclose(fp);
      free(full_file_path);
    }
  }

  closedir(dir);

  /* faup_snapshot_debug(snapshot); */
  
  return snapshot;
}

void faup_snapshot_value_count_write(faup_snapshot_value_count_t *vc, FILE *fp)
{
  size_t value_len;

  value_len=strlen(vc->value);
  fwrite(&value_len, sizeof(size_t), 1, fp);
  fwrite(vc->value, value_len, 1, fp);
  fwrite(&vc->count, sizeof(size_t), 1, fp);
  fwrite(&vc->first_time_seen, sizeof(time_t), 1, fp);
  fwrite(&vc->last_time_seen, sizeof(time_t), 1, fp);
}

void faup_snapshot_item_write(faup_snapshot_item_t *item, FILE *fp)
{
  size_t counter;
  struct htable_iter iter;
  faup_snapshot_value_count_t *vc;

  for (vc = htable_first(&item->values, &iter); vc; vc = htable_next(&item->values, &iter)) {
      faup_snapshot_value_count_write(vc, fp);
    }
}

void faup_snapshot_info_write(char *snapshot_dir)
{
  FILE *fp;
  time_t current;
  char *info_file;
  int retval;
  
  current = time(NULL);

  retval = asprintf(&info_file, "%s%c%s", snapshot_dir, FAUP_OS_DIRSEP_C, "_info");
  fp = fopen(info_file, "wb+");
  fwrite(&current, sizeof(time_t), 1, fp);  
  fclose(fp);

  free(info_file);
}

int faup_snapshot_write(faup_snapshot_t *snapshot, char *workdir)
{
  int retval;
  size_t counter;
  FILE *fp;
  char *item_file;
  char *full_dir_path;
  
  if (workdir) {
    retval = asprintf(&full_dir_path, "%s%c%s", workdir, FAUP_OS_DIRSEP_C, snapshot->name);
  } else {
    full_dir_path=snapshot->name;
  }
  
  retval = mkdir(full_dir_path, 0700);
  if (retval < 0) {
    fprintf(stderr, "Error making directory %s: %s\n", full_dir_path, strerror(errno));
    return -1;
  }

  faup_snapshot_info_write(full_dir_path);
  
  for (counter = 0; counter < snapshot->length; counter++) {
    retval = asprintf(&item_file, "%s%c%s", full_dir_path, FAUP_OS_DIRSEP_C, snapshot->items[counter]->key);
    fp = fopen(item_file, "wb+");
    faup_snapshot_item_write(snapshot->items[counter], fp);
    fclose(fp);
    free(item_file);
  }

  if (workdir) {
    free(full_dir_path);
  }
  
  return 0;
}

/*
 * Return a snapshot object of everything we have in Snapshot B which is 
 * not in Snapshot A.
 */
faup_snapshot_t *faup_snapshot_compare(char *snapshot_dir_a, char *snapshot_dir_b)
{
  faup_snapshot_t *snapshot_a;
  faup_snapshot_t *snapshot_b;

  faup_snapshot_t *result;
  
  faup_snapshot_item_t *item;
  faup_snapshot_item_t *item_a;
  faup_snapshot_item_t *item_b;
  faup_snapshot_item_t *newitem;
  
  size_t counter;
  size_t counter_vc_a;
  size_t counter_vc_b;
  size_t counter_newitem;

  struct htable_iter iter_a;
  struct htable_iter iter_b;

  faup_snapshot_value_count_t *vc_a;
  faup_snapshot_value_count_t *vc_b;
  faup_snapshot_value_count_t *vc_copy;
  
  int found;
  int retval;
  
  result = faup_snapshot_new();
  
  snapshot_a = faup_snapshot_read(snapshot_dir_a);
  if (!snapshot_a) {
    fprintf(stderr, "Error: cannot build snapshot from dir '%s'\n", snapshot_dir_a);
    faup_snapshot_free(result);
    return NULL;
  }

  snapshot_b = faup_snapshot_read(snapshot_dir_b);
  if (!snapshot_b) {
    fprintf(stderr, "Error: cannot build snapshot from dir '%s'\n", snapshot_dir_b);
    faup_snapshot_free(result);
    return NULL;
  }
  
  retval = asprintf(&result->name, "%s-%s", snapshot_b->name, snapshot_a->name);
  for (counter = 0; counter < snapshot_b->length; counter++) {
    item = faup_snapshot_item_get(snapshot_a, snapshot_b->items[counter]->key);
    item_b = faup_snapshot_item_get(snapshot_b, snapshot_b->items[counter]->key);
      
    newitem = faup_snapshot_item_new(item_b->key);
    vc_b = htable_first(&item_b->values, &iter_b);
    while (vc_b) {
      vc_a = faup_snapshot_value_count_get(item, vc_b->value);
      if (!item) {
	vc_copy = faup_snapshot_value_count_copy(vc_b);
	faup_snapshot_value_count_append_object(newitem, vc_copy);
      } else {
	if (!vc_a) {
	  vc_copy = faup_snapshot_value_count_copy(vc_b);
	  faup_snapshot_value_count_append_object(newitem, vc_copy);
	}
      }
	
      vc_b = htable_next(&item_b->values, &iter_b);     
    } // while (vc_b)

    if (newitem->length > 0) {
      faup_snapshot_append_item(result, snapshot_b->items[counter]->key, newitem);
    } else {
      faup_snapshot_item_free(newitem);
    }
  } // for (counter = 0; counter < snapshot_b->length; counter++) {

  faup_snapshot_free(snapshot_a);
  faup_snapshot_free(snapshot_b);
 
  return result;
}
