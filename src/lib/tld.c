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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <faup/faup.h>
#include <faup/tld.h>

#include <faup/utarray.h>

static UT_array *_tlds = NULL;

// FIXME: We should make this code work for native Windows API
int faup_tld_download_mozilla_list(char *store_to_file) 
{
	int sockfd, n;
	char recvbuf[MAX_RECVBUF];
	struct sockaddr_in sin;

	FILE *fileptr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "(socket) Cannot connect outside.\n");
		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(MOZILLA_TLD_LIST_PORT);
	if (inet_pton(AF_INET, MOZILLA_TLD_LIST_IP, &sin.sin_addr) <= 0) {
		fprintf(stderr, "(inet_pton) Cannot connect outside.\n");
		return -1;
	}

	if (connect(sockfd, (const struct sockaddr *) &sin, sizeof(sin)) < 0) {
		fprintf(stderr, "(connect) Cannot connect outside.\n");
		return -1;
	}


	n = send(sockfd, MOZILLA_TLD_LIST_GET, strlen(MOZILLA_TLD_LIST_GET), 0);
	if (n < 0) {
		fprintf(stderr, "(send) Cannot send data.\n");
		return -1;
	} else {
		fileptr = fopen(store_to_file, "w");
		if (!fileptr) {
			fprintf(stderr, "Cannot open file %s to store the TLD list!\n", store_to_file);
			return -1;
		}
	}

	while ( (n=recv(sockfd, recvbuf, MAX_RECVBUF, 0)) > 0) {
		recvbuf[n] = '\0';
		if (fputs(recvbuf, fileptr) == EOF) {
			fprintf(stderr, "(fputs) Cannot get data.\n");
		}
	}
	if (n < 0) {
		fprintf(stderr, "(read) Cannot read data.\n");
	}

	close(sockfd);
	fclose(fileptr);

	return 0;
}

char *faup_tld_home_file_exists(char *append)
{
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

	return NULL;
}

char *faup_tld_get_file_from_home(char *append)
{
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

	return NULL;
}

char *faup_tld_get_file(char *append)
{
	char *dataenv_dir;
	char *retbuf;
	int retval;

	dataenv_dir = getenv("FAUP_DATA_DIR");
	if (!dataenv_dir) {

		retbuf = faup_tld_home_file_exists(append);
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

char *faup_tld_file_to_write(void) 
{
	char *tld_file = NULL;
	tld_file = faup_tld_get_file("mozilla.tlds");
	if (tld_file) {
		FILE *fp;
		fp = fopen(tld_file, "w");
		if (!fp) {
		        free(tld_file);
			return faup_tld_get_file_from_home("mozilla.tlds");
		} else {
			return tld_file;
			fclose(fp);
		}
	}

	return faup_tld_get_file_from_home("mozilla.tlds");
}

int faup_tld_update(void)
{
	char *tld_file;

	tld_file = faup_tld_file_to_write();
	faup_tld_download_mozilla_list(tld_file);

	free(tld_file);

	return 0;
}

void faup_tld_array_populate(void)
{

  return;
	FILE *fp;
	char *tld_file = faup_tld_get_file("mozilla.tlds");

	if (_tlds) {
		fprintf(stderr, "The tld array has already been populated!\n");
		free(tld_file);
		return;
	}
	utarray_new(_tlds, &ut_str_icd);

	fp = fopen(tld_file, "r");
	free(tld_file);
	if (fp) {
		char line[524]; // We have the control over our tld file

		while(fgets(line, sizeof(line), fp) != NULL) {
			size_t line_len = strlen(line);

			switch(line[0]) {
				case '/':
				case '\n':
				case '\r':
				case '0':
				case ' ':
					break;
				default:
					if (!isupper(line[0])) { // Last check: Apache banners starts with upper case, not TLDs
						if (line_len > 0) {
							line[line_len - 1] = '\0';
							char *allocated_line = strdup((const char *)line);
							utarray_push_back(_tlds, &allocated_line);
						}						
					}
			}
		}
		fclose(fp);
	}

}

void faup_tld_array_destroy(void)
{
    if (_tlds) {
	utarray_free(_tlds);
    }
}

void faup_tld_array_cb_to_stdout(char *tld, void *user_data)
{
	fprintf(stdout, "tld:%s\n", tld);
}

void faup_tld_array_foreach(void (*cb_tld_array)(char *tld, void *user_data), void *user_data)
{
	char **p_tld;

	if (!_tlds) {
	  fprintf(stderr, "Error: the TLD array has not been populated!\n");
	  return;
	}

	p_tld = NULL;
	while ((p_tld = (char **)utarray_next(_tlds, p_tld))) {
		cb_tld_array(*p_tld, user_data);
	}
}


