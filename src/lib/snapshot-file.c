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
#include <faup/snapshot-file.h>
#include <faup/utils.h>

#include <miniz.h>
#include <miniz_zip.h>

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
  int retval;
  size_t dirpath_len;

  if (!dirpath) { return NULL; }
  
  dirpath_len = strlen(dirpath);
  if (dirpath_len > 5) {
    if (!strcmp(&dirpath[dirpath_len - 5], ".urls")) {
      dirpath[dirpath_len - 5] = '\0';
    }
  }

  retval = faup_snapshot_file_unzip(dirpath);
  if (retval < 0) {
    fprintf(stderr, "Cannot read snapshot '%s'\n", dirpath);
    return NULL;
  }

  dir = opendir(dirpath);
  if (!dir) {
    fprintf(stderr, "Error reading directory '%s': %s\n", dirpath, strerror(errno));
    return NULL;
  }
  snapshot = faup_snapshot_open(dirpath);
  while ((ent = readdir(dir)) != NULL) {
    if ((ent->d_name[0] != '\0') && (ent->d_name[0] != '.') && (ent->d_name[0] != '_')) {
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

  retval = faup_utils_remove_dir(dirpath);
  if (retval < 0) {
    fprintf(stderr, "Cannot remove temporary directory '%s'\n", dirpath);
  }
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
    full_dir_path = snapshot->name;
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

  // will create a file <full_dir_path>.urls (url snapshot)
  faup_snapshot_file_zip(full_dir_path);

  // now it is zipped and can be shared as a single file,
  retval = faup_utils_remove_dir(full_dir_path);
  if (retval < 0) {
    fprintf(stderr, "Error removing '%s': %s\n", full_dir_path, strerror(errno));
  }
  
  if (workdir) {
    free(full_dir_path);
  }
  
  return 0;
}

int faup_snapshot_file_zip(char *dirpath)
{
  DIR *dir;
  struct dirent *ent;
  FILE *fp;

  char *zip_filename;
  long file_size;
  void *file_content;

  mz_bool status;
  mz_zip_archive zip_archive;

  struct stat zipstats;
  int retval;

  dir = opendir(dirpath);
  if (!dir) {
    fprintf(stderr, "Error reading directory '%s': %s\n", dirpath, strerror(errno));
    return -1;
  }

  retval = asprintf(&zip_filename, "%s.urls", dirpath);
  retval = stat(zip_filename, &zipstats);
  if (retval == 0) {
    fprintf(stderr, "The snapshot %s already exists!\n", zip_filename);
    free(zip_filename);
    closedir(dir);
    return -1;
  }

  while ((ent = readdir(dir)) != NULL) {
      char *full_file_path;
      int retval;
      size_t readval;

      if (ent->d_name[0] == '.') { continue; }

      retval = asprintf(&full_file_path, "%s%c%s", dirpath, FAUP_OS_DIRSEP_C, ent->d_name);
      fp = fopen(full_file_path, "rb");
      fseek(fp, 0, SEEK_END);
      file_size = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      
      file_content = malloc(file_size + 1);
      if (!file_content) {
	fprintf(stderr, "Cannot allocate the file size content:%lu", file_size);
      }

      readval = fread(file_content, file_size, 1, fp);
      status = mz_zip_add_mem_to_archive_file_in_place(zip_filename, full_file_path, file_content, file_size, NULL, 0, MZ_BEST_COMPRESSION);
      if (!status) {
      	fprintf(stderr, "Cannot create %s!\n", full_file_path);
      	return -1;
      }

      /* printf("file name:%s\n", full_file_path); */
      
      fclose(fp);
      free(file_content);
      free(full_file_path);
  }

  free(zip_filename);

  closedir(dir);
  
  return 0;
}

int faup_snapshot_file_unzip(char *zipfile)
{

  char *url_snapshot_file = NULL;
  size_t zipfile_len;
  int retval;

  mz_zip_archive zip_archive;
  mz_bool status;
  int i;

  retval = asprintf(&url_snapshot_file, "%s.urls", zipfile);

  memset(&zip_archive, 0, sizeof(zip_archive));
  status = mz_zip_reader_init_file(&zip_archive, url_snapshot_file, 0);
  for (i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++) {
    mz_zip_archive_file_stat file_stat;

    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
      fprintf(stderr, "Cannot stat the zip file!\n");
      mz_zip_reader_end(&zip_archive);
      return -1;
    }

    faup_utils_recursive_mkdir(file_stat.m_filename);

    status = mz_zip_reader_extract_to_file(&zip_archive, i, file_stat.m_filename, 0);
    if (!status) {
      fprintf(stderr, "Error extracting '%s': %s\n", file_stat.m_filename, strerror(errno));
    }
  }

  mz_zip_reader_end(&zip_archive);

  free(url_snapshot_file);

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
      } // if (!item)
	
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
