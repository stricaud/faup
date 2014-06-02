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

#ifdef WIN32
 	#include <winsock2.h>
 	#pragma comment(lib,"ws2_32.lib")
#else
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/stat.h>
#endif

#ifdef LINUX
#define __USE_BSD
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#endif // WIN32

#include <faup/faup.h>
#include <faup/datadir.h>
#include <faup/tld.h>
#include <faup/compat.h>

#include <faup/utarray.h>

static UT_array *_tlds = NULL;

// FIXME: We should make this code work for native Windows API
int faup_tld_download_mozilla_list(char *store_to_file) 
{
	int sockfd, n;
	char recvbuf[MAX_RECVBUF];
	struct sockaddr_in sin;

	FILE *fileptr;

#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);
#endif

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

#ifdef WIN32
	closesocket(sockfd);
	WSACleanup();
#else
	close(sockfd);
#endif

	fclose(fileptr);

	return 0;
}

int faup_tld_update(void)
{
	char *tld_file;

	tld_file = faup_datadir_file_to_write("mozilla.tlds", false);
	if (tld_file) {
		faup_tld_download_mozilla_list(tld_file);
		free(tld_file);
	} else {
		return -1;
	}

	return 0;
}

int faup_tld_array_populate(void)
{

	FILE *fp;
	char *tld_file = faup_datadir_get_file("mozilla.tlds", false);
	bool begin_icann_domains = 0;

	if (_tlds) {
		fprintf(stderr, "The tld array has already been populated!\n");
		free(tld_file);
		return -1;
	}
	utarray_new(_tlds, &ut_str_icd);

	fp = fopen(tld_file, "r");
	free(tld_file);
	if (fp) {
		char line[524]; // We have the control over our tld file

		while(fgets(line, sizeof(line), fp) != NULL) {
			size_t line_len = strlen(line);
			char *allocated_line = NULL;

			if (line_len > 0) {
				line[line_len - 1] = '\0';
				allocated_line = strdup((const char *)line);

				switch(line[0]) {
					case '/':
						if (!strncmp("// ===BEGIN ICANN DOMAINS===", allocated_line, 28)) {
							begin_icann_domains = 1;
						}
						break;
					case '\n':
					case '\r':
					case '0':
					case ' ':
					case '\0':
						break;
					default:
						if (line_len > 0) {
							if (begin_icann_domains) {
								utarray_push_back(_tlds, &allocated_line);
							}
						} // if (line_len > 0) {			
				} // switch(line[0]) {
				free(allocated_line);
			} // if (line_len > 0) {
		}
		fclose(fp);
	}

	return 0;
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

void faup_tld_datadir_print(void) 
{
	printf("FAUP_DATA_DIR=%s\n", FAUP_DATA_DIR);
	printf("TLD file being used:%s\n", faup_datadir_get_file("mozilla.tlds", false));
}

