/*
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * Version 2, December 2004 
 *
 * Copyright (C) 2012-2013 Sebastien Tricaud <sebastien@honeynet.org> 
 *
 * Everyone is permitted to copy and distribute verbatim or modified 
 * copies of this license document, and changing it is allowed as long 
 * as the name is changed. 
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#ifdef LINUX
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif

#include <faup/faup.h>
#include <faup/datadir.h>

#ifndef WIN32
#include <fcntl.h>

/* Home directory of the current user, or NULL. getpwuid() has no entry for
 * the uid in many containers, so fall back on $HOME. */
static const char *faup_datadir_homedir(void)
{
	struct passwd *pw = getpwuid(getuid());
	if (pw && pw->pw_dir && pw->pw_dir[0]) {
		return pw->pw_dir;
	}
	return getenv("HOME");
}
#endif

char *faup_datadir_home_file_or_dir_exists(char *append)
{
#ifndef WIN32
	int retval;
	char *retbuf;
	const char *homedir = faup_datadir_homedir();
	struct stat append_stat;

	if (!homedir) {
		return NULL;
	}
	retval = asprintf(&retbuf, "%s%s.faup%s%s", homedir, FAUP_OS_DIRSEP, FAUP_OS_DIRSEP, append);
	if (retval < 0) {
		fprintf(stderr, "Cannot allocate in %s with parameter '%s'\n", __FUNCTION__, append);
		return NULL;
	}

	retval = stat(retbuf, &append_stat);
	if (retval < 0) {
		// The file or dir is not found
		free(retbuf);
		return NULL;
	} else {
		return retbuf;
	}

	free(retbuf);
#endif // WIN32
	return NULL;
}

char *faup_datadir_get_file_from_home(char *append, bool to_symlink)
{
#ifndef WIN32
	int retval;
	char *retbuf;
	const char *homedir = faup_datadir_homedir();
	int fd;

	if (!homedir) {
		return NULL;
	}
	retval = asprintf(&retbuf, "%s%s.faup", homedir, FAUP_OS_DIRSEP);
	if (retval < 0) {
		return NULL;
	}
	retval = mkdir(retbuf, 0700);
	free(retbuf);

	retval = asprintf(&retbuf, "%s%s.faup%s%s", homedir, FAUP_OS_DIRSEP, FAUP_OS_DIRSEP, append);
	if (retval < 0) {
		return NULL;
	}

	/* Check that we can create the file there without truncating or
	 * removing anything that already exists at that path. */
	fd = open(retbuf, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
	if (fd >= 0) {
		close(fd);
		if (to_symlink) {
			/* Only a placeholder we just created; the caller symlinks over it */
			unlink(retbuf);
		}
		return retbuf;
	}
	if (errno == EEXIST) {
		struct stat st;
		if (to_symlink) {
			/* Replace an existing module symlink, never a real file */
			if ((lstat(retbuf, &st) == 0) && S_ISLNK(st.st_mode) && (unlink(retbuf) == 0)) {
				return retbuf;
			}
		} else if (access(retbuf, W_OK) == 0) {
			return retbuf;
		}
	}
	free(retbuf);
#endif
	return NULL;
}

bool faup_datadir_make_dir_from_home(char *append)
{
#ifndef WIN32
	int retval;
	char *retbuf;
	const char *homedir = faup_datadir_homedir();

	if (!homedir) {
		return false;
	}
	retval = asprintf(&retbuf, "%s%s.faup", homedir, FAUP_OS_DIRSEP);
	if (retval < 0) {
		return false;
	}
	retval = mkdir(retbuf, 0700);
	if (retval) {
	  if (errno != EEXIST) {
	    free(retbuf);
	    return false;
	  }
	}
	free(retbuf);

	retval = asprintf(&retbuf, "%s%s.faup%s%s", homedir, FAUP_OS_DIRSEP, FAUP_OS_DIRSEP, append);
	if (retval < 0) {
		return false;
	}
	retval = mkdir(retbuf, 0700);
	if (retval) {
	  if (errno != EEXIST) {
	    free(retbuf);
	    return false;
	  }
	}
	free(retbuf);
#endif
	return true;
}

char *faup_datadir_get_file(char *append, bool to_symlink)
{
	char *dataenv_dir;
	char *retbuf;
	int retval;

	dataenv_dir = getenv("FAUP_DATA_DIR");
	if (!dataenv_dir) {

		if (!to_symlink) {
			retbuf = faup_datadir_home_file_or_dir_exists(append);
			if (retbuf) {
				return retbuf;
			}
		}

		return faup_datadir_get_global_file(append);
	}

	if (strlen(dataenv_dir) > FAUP_MAXPATHLEN) {
		fprintf(stderr, "Invalid path!\n");
		return NULL; /* Invalid path! */
	}
	retval = asprintf(&retbuf, "%s%s%s", dataenv_dir, FAUP_OS_DIRSEP, append);

	return retbuf;
}

char *faup_datadir_get_global_file(char *append)
{
	char *retbuf;
	int retval;

	retval = asprintf(&retbuf, "%s%s", FAUP_DATA_DIR, append);
	return retbuf;
}

char *faup_datadir_file_to_write(char *file, bool to_symlink) 
{
	char *datadir_file = NULL;
	datadir_file = faup_datadir_get_file(file, to_symlink);
	if (datadir_file) {
		FILE *fp;
		fp = fopen(datadir_file, "w");
		if (!fp) {
		    free(datadir_file);
			return faup_datadir_get_file_from_home(file, to_symlink);
		} else {
			return datadir_file;
			fclose(fp);
		}
	}

	return faup_datadir_get_file_from_home(file, to_symlink);
}


