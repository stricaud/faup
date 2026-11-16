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
#define _GNU_SOURCE // asprintf() and friends on glibc and musl
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#include <sys/wait.h>
#endif // WIN32

#include <faup/faup.h>
#include <faup/datadir.h>
#include <faup/tld.h>
#include <faup/compat.h>

#include <faup/utarray.h>

static UT_array *_tlds = NULL;

/*
 * Download the public suffix list with curl (or $FAUP_DOWNLOAD_PROGRAM, which receives
 * "<url> --output <file>") into a temporary file, and replace the current list only
 * when the download succeeded: a failure must not leave an empty or HTML "list"
 * behind (issues #79 and #81). Returns 0 on success, -1 otherwise.
 */
int faup_tld_download_mozilla_list(char *store_to_file) 
{
#ifdef WIN32
	fprintf(stderr, "Downloading the TLD list is not supported on Windows. Fetch %s to %s yourself.\n", MOZILLA_TLD_LIST_URL, store_to_file);
	return -1;
#else
	char *download_program;
	char *tmp_file = NULL;
	char *curl_argv[] = { NULL, "--fail", "--silent", "--show-error", "--location", MOZILLA_TLD_LIST_URL, "--output", NULL, NULL };
	char *custom_argv[] = { NULL, MOZILLA_TLD_LIST_URL, "--output", NULL, NULL };
	char **newargv;
	pid_t pid;
	int status;
	struct stat st;
	int retval;

	download_program = getenv("FAUP_DOWNLOAD_PROGRAM");
	retval = asprintf(&tmp_file, "%s.download", store_to_file);
	if (retval < 0) {
		fprintf(stderr, "Cannot allocate the temporary download path\n");
		return -1;
	}
	if (!download_program) {
		download_program = "/usr/bin/curl";
		curl_argv[0] = download_program;
		curl_argv[7] = tmp_file;
		newargv = curl_argv;
	} else {
		custom_argv[0] = download_program;
		custom_argv[3] = tmp_file;
		newargv = custom_argv;
	}

	fprintf(stdout, "Downloading new TLD list to %s\n", store_to_file);

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Cannot fork to run %s: %s\n", download_program, strerror(errno));
		free(tmp_file);
		return -1;
	}
	if (pid == 0) {
		execv(download_program, newargv);
		fprintf(stderr, "Cannot run %s: %s\n", download_program, strerror(errno));
		_exit(127);
	}
	if (waitpid(pid, &status, 0) < 0) {
		fprintf(stderr, "Cannot wait for %s: %s\n", download_program, strerror(errno));
		unlink(tmp_file);
		free(tmp_file);
		return -1;
	}
	if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0)) {
		fprintf(stderr, "Download failed (%s exited with status %d). Keeping the current %s.\n",
			download_program, WIFEXITED(status) ? WEXITSTATUS(status) : -1, store_to_file);
		unlink(tmp_file);
		free(tmp_file);
		return -1;
	}
	/* The real list is several hundred KB; anything tiny is an error page or an empty file */
	if ((stat(tmp_file, &st) < 0) || (st.st_size < 10000)) {
		fprintf(stderr, "Download of %s produced no usable list. Keeping the current %s.\n", MOZILLA_TLD_LIST_URL, store_to_file);
		unlink(tmp_file);
		free(tmp_file);
		return -1;
	}
	if (rename(tmp_file, store_to_file) < 0) {
		fprintf(stderr, "Cannot install the new list as %s: %s\n", store_to_file, strerror(errno));
		unlink(tmp_file);
		free(tmp_file);
		return -1;
	}
	free(tmp_file);

	return 0;
#endif
}

int faup_tld_update(void)
{
	char *tld_file;
	int retval;

	tld_file = faup_datadir_file_to_write(faup_tld_get_filename(), false);
	if (!tld_file) {
		fprintf(stderr, "Cannot find a writable location for the TLD list\n");
		return -1;
	}
	retval = faup_tld_download_mozilla_list(tld_file);
	free(tld_file);

	return retval;
}

int faup_tld_array_populate(void)
{

	FILE *fp;
	char *tld_file = faup_datadir_get_file(faup_tld_get_filename(), false);
	bool begin_icann_domains = 0;

	if (_tlds) {
		fprintf(stderr, "The tld array has already been populated!\n");
		free(tld_file);
		return -1;
	}
	if (!tld_file) {
		fprintf(stderr, "Cannot locate the TLD file\n");
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

			/* Strip the end of line only. A last line without a newline
			 * or a line filling the buffer must not lose its last char. */
			while ((line_len > 0) && ((line[line_len - 1] == '\n') || (line[line_len - 1] == '\r'))) {
				line[--line_len] = '\0';
			}
			if (line_len > 0) {
				allocated_line = strdup((const char *)line);

				switch(line[0]) {
					case '/':
						if (!strncmp("// ===BEGIN ICANN DOMAINS===", allocated_line, 28)) {
							begin_icann_domains = 1;
						}
						break;
					case '\n':
					case '\r':
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
	_tlds = NULL;
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
	char *tld_file = faup_datadir_get_file(faup_tld_get_filename(), false);
	printf("TLD file being used:%s\n", tld_file ? tld_file : "(not found)");
	free(tld_file);
}

char *faup_tld_get_filename(void)
{   
	char *tld_filename;
	
	if ((tld_filename = getenv("FAUP_TLD_FILE")) != NULL) {
		return tld_filename;
	} else {
		return "mozilla.tlds";
	}
}
