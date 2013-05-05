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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <faup/faup.h>
#include <faup/tld.h>

// FIXME: We should make this code work for native Windows API
int faup_tld_get_mozilla_list(char *store_to_file) 
{
	int sockfd, n;
	char recvbuf[MAX_SOCKBUF_LINE + 1];
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

	while ( (n=read(sockfd, recvbuf, MAX_SOCKBUF_LINE)) > 0) {
		recvbuf[n] = '\0';
		if (fputs(recvbuf, fileptr) == EOF) {
			fprintf(stderr, "(fputs) Cannot get data.\n");
		}
	}
	if (n < 0) {
		fprintf(stderr, "(read) Cannot read data.\n");
	}

	fclose(fileptr);

	return 0;
}

char *faup_tld_get_file_from_home(char *append)
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

char *faup_tld_get_file(char *append)
{
	char *dataenv_dir;
	char *retbuf;
	int retval;

	dataenv_dir = getenv("FAUP_DATA_DIR");
	if (!dataenv_dir) {

		retbuf = faup_tld_get_file_from_home(append);
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


