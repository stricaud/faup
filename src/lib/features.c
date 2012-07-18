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

void furl_features_init(furl_features_t* features)
{
	features->scheme.pos        = -1;
	features->hierarchical.pos  = -1;
	features->credential.pos    = -1;
	features->host.pos          = -1;
	features->subdomain.pos     = -1;
	features->domain.pos        = -1;
	features->port.pos          = -1;
	features->resource_path.pos = -1;
	features->query_string.pos  = -1;
	features->fragment.pos      = -1;
	features->tld.pos           = -1;
}

static inline char get_last_c(const char *buffer, size_t pos)
{
	if (pos > 0) {
		return buffer[pos-1];
	}

	return -1;
}

int furl_features_exist(furl_feature_t feature) 
{
	return (feature.pos >= 0);
}

int furl_features_errors_lookup(furl_features_t const* url_features) 
{
	/* if ((furl_features_exist(url_features.scheme)) && */
	/*     (!furl_features_exist(url_features.hierarchical))) { */
	/* 	fprintf(stderr, "url error: can't have a scheme without a hierarchical!\n"); */
	/* 	return 1; */
	/* } */

	return 0;
}

void furl_features_find(furl_handler_t *fh, const char *url, const size_t url_len)
{
	furl_features_t* url_features = &fh->furl.features;
	furl_features_init(url_features);
	char c;
	size_t nb_slashes = 0;
	//int char_counter[128];
	int last_slash_pos = 0;
	const char *url_o = url;	/* We keep the original pointer as we move it */
	ssize_t whatever_len = 0;

	furl_last_slash_t last_slash_meaning = FURL_LAST_SLASH_NOTFOUND;

	ssize_t current_pos = 0;
	ssize_t buffer_pos = 0;

	for (size_t i = 0; i < url_len; i++) {
		c = url[i];
		if (c == '/') {
			nb_slashes++;
		}

		switch(c) {
			case '/':
				/* If it is the first time we have a '/' and previous char is ':' */
				if ((nb_slashes == 1) && (get_last_c(url_o, current_pos) == ':')) {
					if (last_slash_meaning < FURL_LAST_SLASH_AFTER_DOMAIN) {
						last_slash_meaning = FURL_LAST_SLASH_HIERARCHICAL;
						url_features->hierarchical.pos = current_pos -1;
						c = get_last_c(url_o, current_pos - 1);
						if (isalpha(c)) {
							url_features->scheme.pos = 0;
						}
						url_features->host.pos = -1; /* So finally we don't start with a host */
						url_features->port.pos = -1; /* So the last ':' we've found was not for a port but for  */
					} /* if (last_slash_meaning < FURL_LAST_SLASH_AFTER_DOMAIN) */
				} else {
					/* We now check for the resource path */
					if (!furl_features_exist(url_features->resource_path)) {
						if (!furl_features_exist(url_features->scheme)) {
							if (!furl_features_exist(url_features->hierarchical)) {
								/* This host has a '/' with no hierarchy */
								/* The seen '/' is not a hierarchy so it is something like foo/bar.html */
								last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN;
								url_features->resource_path.pos = current_pos;
							}
						} else {
							if (furl_features_exist(url_features->host)) {
								last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN;
								url_features->resource_path.pos = current_pos;		
							}
						}
					}
				}

				last_slash_pos = current_pos;

				if (furl_features_exist(url_features->host)) {
					last_slash_meaning = FURL_LAST_SLASH_AFTER_DOMAIN;
				}

				buffer_pos=-1;
				break;
			case '@':
				if (!furl_features_exist(url_features->credential)) {
					//fh->allocated_buf[buffer_pos] = '\0';
					whatever_len = buffer_pos;
					if ((last_slash_meaning == FURL_LAST_SLASH_HIERARCHICAL) || /* This '@' belongs to the authentication if http://foo:bar@host/blah */
							(last_slash_meaning == FURL_LAST_SLASH_NOTFOUND)) {     /* This '@' belongs to the authentication if foo:bar@host/blah */
						url_features->credential.pos = current_pos - whatever_len;
						url_features->host.pos = current_pos + 1;
						url_features->port.pos = -1; /* So the last ':' we've found was not for a port but for credential */
					}
				}
				buffer_pos=-1;
				break;
			case ':':
				/* We have three cases here:
				   - a ':' for the credential
				   - a ':' for the port number
				   - a ':' in the query request */
				/* url_features->port = -1; */
				if (!furl_features_exist(url_features->port)) {
					if (last_slash_meaning < FURL_LAST_SLASH_AFTER_DOMAIN) {
						//fh->allocated_buf[buffer_pos] = c;
						url_features->port.pos = current_pos + 1;
					}
				}

				buffer_pos=-1;
				break;
			case '?':
				url_features->query_string.pos = current_pos;

				buffer_pos=-1;
				break;
			case '#':
				url_features->fragment.pos = current_pos;

				buffer_pos=-1;
				break;
			default:
				//fh->allocated_buf[buffer_pos] = c;
				if (current_pos == 0) {
					/* We assume we have a host to start. We shall turn it back to -1 if we have a scheme or hierachy */
					url_features->host.pos = 0;
				}
				/* We have a scheme, but no host nor no credential, then host is current_pos until we have a credential to remove it */
				if ((!furl_features_exist(url_features->host)) && 
						(!furl_features_exist(url_features->credential))) {

					url_features->host.pos = current_pos;

				}

				break;
		}

		buffer_pos++;
		current_pos++;
	}
}

void furl_features_show(furl_handler_t const* fh, const furl_feature_t feature, FILE* out)
{
	if (furl_features_exist(feature)) {
		fwrite(fh->furl.org_str + feature.pos, feature.size, 1, out);
	}
}

void furl_features_debug(const char *url, furl_features_t const* features)
{
	fprintf(stdout, "url:%s\n", url);
	fprintf(stdout, "features->scheme:%d,%u\n", features->scheme.pos, features->scheme.size);
	fprintf(stdout, "features->hierarchical:%d,%u\n", features->hierarchical.pos, features->hierarchical.size);
	fprintf(stdout, "features->credential:%d,%u\n", features->credential.pos, features->credential.size);
	fprintf(stdout, "features->host:%d,%u\n", features->host.pos, features->host.size);
	fprintf(stdout, "features->port:%d,%u\n", features->port.pos, features->port.size);
	fprintf(stdout, "features->resource_path:%d,%u\n", features->resource_path.pos, features->resource_path.size);
	fprintf(stdout, "features->query_string:%d,%u\n", features->query_string.pos, features->query_string.size);
	fprintf(stdout, "features->fragment:%d,%u\n", features->fragment.pos, features->fragment.size);
}
