/*
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * Version 2, December 2004 
 *
 * Copyright (C) 2012 Sebastien Tricaud <sebastien@honeynet.org> 
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

#include <furl/furl.h>
#include <furl/features.h>

#include <stdio.h>
#include <ctype.h>

furl_features_t furl_features_init(void)
{
	furl_features_t features;

	features.scheme        = -1;
	features.hierarchical  = -1;
	features.credential    = -1;
	features.domain        = -1;
	features.port          = -1;
	features.resource_path = -1;
	features.query_string  = -1;
	features.fragment      = -1;

	return features;
}

static char get_last_c(const char *buffer, size_t pos)
{
	if (pos > 0) {
		return buffer[pos-1];
	}

	return -1;
}

int furl_features_exist(int feature) 
{
	if (feature >= 0) {
		return 1;
	}

	return 0;
}

int furl_features_errors_lookup(furl_features_t url_features) 
{
	if ((furl_features_exist(url_features.scheme)) && 
	    (!furl_features_exist(url_features.hierarchical))) {
		fprintf(stderr, "url error: can't have a schem without a hierarchical!\n");
		return 1;
	}

	return 0;
}

furl_features_t furl_features_find(furl_handler_t *fh, char *url, size_t url_len)
{
	furl_features_t url_features = furl_features_init();
	char c;
	int char_counter[128] = {};
	int last_slash_pos = 0;
	const char *url_o = url;	/* We keep the original pointer as we move it */
	size_t whatever_len = 0;

	furl_last_slash_t last_slash_meaning = FURL_LAST_SLASH_NOTFOUND;

	size_t current_pos = 0;
	size_t buffer_pos = 0;
	while (c=*url++) {
		/* fh->allocated_buf[buffer_position] = c; */
		char_counter[c]++;

			switch(c) {
			case '/':
				/* If it is the first time we have a '/' and previous char is ':' */
				if ((char_counter['/'] == 1) && (get_last_c(url_o, current_pos) == ':')) {
					last_slash_meaning = FURL_LAST_SLASH_HIERARCHICAL;
					url_features.hierarchical = current_pos -1;
					c = get_last_c(url_o, current_pos - 1);
					if (isalpha(c)) {
						url_features.scheme = 0;
					}
					url_features.domain = -1; /* So finally we don't start with a domain */
				} else {
					if (!furl_features_exist(url_features.scheme)) {
						/* This domain has a '/' with no hierarchy */
						/* The seen '/' is not a hierarchy so it is something like foo/bar.html */
						last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN;
						url_features.resource_path = current_pos;
					} else {
						if (furl_features_exist(url_features.domain)) {
							last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN;
							url_features.resource_path = current_pos;		
						}
					}
				}

				last_slash_pos = current_pos;

				if (furl_features_exist(url_features.domain)) {
					last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN;
				}

				buffer_pos=-1;
				break;
			case '@':
				if (!furl_features_exist(url_features.credential)) {
					fh->allocated_buf[buffer_pos] = '\0';
					whatever_len = strlen(fh->allocated_buf);
					if ((last_slash_meaning == FURL_LAST_SLASH_HIERARCHICAL) || /* This '@' belongs to the authentication if http://foo@bar:domain/blah */
				     	    (last_slash_meaning == FURL_LAST_SLASH_NOTFOUND)) {     /* This '@' belongs to the authentication if foo@bar:domain/blah */
						url_features.credential = current_pos - whatever_len;
						url_features.domain = -1;
					}
				}
				buffer_pos=-1;
				break;
			case ':':
				/* We have three cases here:
				   - a ':' for the credential
				   - a ':' for the port number
				   - a ':' in the query request */

				/* If we have a domain, but have not seen the '/' after the domain, then we have a port */
					/* last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN; */
				if ((furl_features_exist(url_features.domain)) && (last_slash_meaning < FURL_LAST_SLASH_AFTER_DOMAIN)) {
					if ((current_pos + (url_len - current_pos)) > (current_pos + 2)) { /* We have enough to check +1, +2 */
						if (url_o[current_pos+1] != '/') {
							/* We are not a scheme, so we are after a real domain and not before a credential (where the domain could look like the scheme until we know it is a scheme) */
							url_features.port = current_pos + 1;
						}
					}
				}

				/* If we have credential but no domain, then we start having a domain now */
				if ((furl_features_exist(url_features.credential)) && (!furl_features_exist(url_features.domain))) {
					url_features.domain = current_pos + 1;
				}

				buffer_pos=-1;
				break;
			case '?':
				url_features.query_string = current_pos;

				buffer_pos=-1;
				break;
			case '#':
				url_features.fragment = current_pos;

				buffer_pos=-1;
				break;
			default:
				fh->allocated_buf[buffer_pos] = c;
				if (current_pos == 0) {
					/* We assume we have a domain to start. We shall turn it back to -1 if we have a scheme or hierachy */
					url_features.domain = 0;
				}
				/* We have a scheme, but no domain nor no credential, then domain is current_pos until we have a credential to remove it */
				if ((!furl_features_exist(url_features.domain)) && 
				    (!furl_features_exist(url_features.credential))) {

					url_features.domain = current_pos;

				}

				break;
			}

		buffer_pos++;
		current_pos++;
	}

	return url_features;
}

void furl_features_debug(char *url, furl_features_t features)
{
	fprintf(stdout, "url:%s\n", url);
	fprintf(stdout, "features.scheme:%d\n", features.scheme);
	fprintf(stdout, "features.hierarchical:%d\n", features.hierarchical);
	fprintf(stdout, "features.credential:%d\n", features.credential);
	fprintf(stdout, "features.domain:%d\n", features.domain);
	fprintf(stdout, "features.port:%d\n", features.port);
	fprintf(stdout, "features.resource_path:%d\n", features.resource_path);
	fprintf(stdout, "features.query_string:%d\n", features.query_string);
	fprintf(stdout, "features.fragment:%d\n", features.fragment);
}
