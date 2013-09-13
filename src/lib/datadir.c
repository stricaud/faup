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

#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif

#include <faup/faup.h>
#include <faup/datadir.h>

char *faup_datadir_home_file_exists(char *append)
{
#ifndef WIN32
	int retval;
	char *retbuf;
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	FILE *fp;

	retval = asprintf(&retbuf, "%s%s.faup%s%s", homedir, FAUP_OS_DIRSEP, FAUP_OS_DIRSEP, append);
	fp = fopen(retbuf, "r");
	if (fp) {
		return retbuf;
	}

	free(retbuf);
#endif // WIN32
	return NULL;
}

char *faup_datadir_get_file_from_home(char *append)
{
#ifndef WIN32
	int retval;
	char *retbuf;
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	FILE *fp;

	retval = asprintf(&retbuf, "%s%s.faup", homedir, FAUP_OS_DIRSEP);
	retval = mkdir(retbuf, 0700);
	free(retbuf);

	retval = asprintf(&retbuf, "%s%s.faup%s%s", homedir, FAUP_OS_DIRSEP, FAUP_OS_DIRSEP, append);
	fp = fopen(retbuf, "w");
	if (fp) {
		return retbuf;
	}
#endif
	return NULL;
}

char *faup_datadir_get_file(char *append)
{
	char *dataenv_dir;
	char *retbuf;
	int retval;

	dataenv_dir = getenv("FAUP_DATA_DIR");
	if (!dataenv_dir) {

		retbuf = faup_datadir_home_file_exists(append);
		if (retbuf) {
			return retbuf;
		}

		retval = asprintf(&retbuf, "%s%s", FAUP_DATA_DIR, append);
		return retbuf;
	}

	if (strlen(dataenv_dir) > FAUP_MAXPATHLEN) {
		return NULL; /* Invalid path! */
	}
	retval = asprintf(&retbuf, "%s%s%s", dataenv_dir, FAUP_OS_DIRSEP, append);

	return retbuf;
}

char *faup_datadir_file_to_write(char *file) 
{
	char *datadir_file = NULL;
	datadir_file = faup_datadir_get_file(file);
	if (datadir_file) {
		FILE *fp;
		fp = fopen(datadir_file, "w");
		if (!fp) {
		    free(datadir_file);
			return faup_datadir_get_file_from_home(file);
		} else {
			return datadir_file;
			fclose(fp);
		}
	}

	return faup_datadir_get_file_from_home(file);
}


