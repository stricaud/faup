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
  
  printf("Reading item %s\n", item_name);


  while (!feof(fp)) {
    vc = faup_snapshot_value_count_new();
    readsize = fread(&value_len, sizeof(size_t), 1, fp);
    if (readsize <= 0) {
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
    if ((ent->d_name) && (ent->d_name[0] != '.')) {
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

  faup_snapshot_debug(snapshot);
  
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

  for (counter = 0; counter < item->length; counter++) {
    faup_snapshot_value_count_write(item->value_count[counter], fp);
  }
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

  for (counter = 0; counter < snapshot->length; counter++) {
    retval = asprintf(&item_file, "%s%c%s", full_dir_path, FAUP_OS_DIRSEP_C, snapshot->items_names[counter]);
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

  faup_snapshot_item_t *item_a;
  faup_snapshot_item_t *item_b;

  size_t counter;

  
  snapshot_a = faup_snapshot_read(snapshot_dir_a);
  if (!snapshot_a) {
    fprintf(stderr, "Error: cannot build snapshot from dir '%s'\n", snapshot_dir_a);
    return NULL;
  }
  snapshot_b = faup_snapshot_read(snapshot_dir_b);
  if (!snapshot_b) {
    fprintf(stderr, "Error: cannot build snapshot from dir '%s'\n", snapshot_dir_b);
    return NULL;
  }

  for (counter = 0; counter < snapshot_a->length; counter++) {

  }
  /* item = faup_snapshot_item_get(snapshot, item_name); */
  
  faup_snapshot_free(snapshot_a);
  faup_snapshot_free(snapshot_b);
  
  return result;
}

