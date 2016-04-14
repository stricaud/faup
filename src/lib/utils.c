#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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
