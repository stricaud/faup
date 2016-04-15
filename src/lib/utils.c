#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <faup/faup.h>
#include <faup/utils.h>

static int removeall_cb(const char *fpath, const struct stat *sb,
			int typeflag, struct FTW *ftwbuf)
{
  int retval;

  retval = remove(fpath);
  if (retval) {
    fprintf(stderr,"Error removing '%s': %s\n", fpath, strerror(errno));
  }
  return retval;
}

int faup_utils_remove_dir(char *dir)
{
  return nftw(dir, removeall_cb, 42, FTW_DEPTH|FTW_MOUNT|FTW_PHYS);
}

int faup_utils_recursive_mkdir(char *filepath)
{
  char tmp[PATH_MAX];
  char *p = NULL;
  size_t len;

  len = strlen(filepath);
  strncpy(tmp, filepath, len);
  
  if (len <= 1) {
    fprintf(stderr, "Error: there is no directory to create from '%s'\n", filepath);
    return -1;
  }

  for (p = tmp; *p; p++) {
    if (*p == FAUP_OS_DIRSEP_C) {
      *p = '\0';
      mkdir(tmp, S_IRWXU);
      *p = FAUP_OS_DIRSEP_C;
    }
  }

  return 0;
}
