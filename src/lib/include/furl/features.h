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

#ifndef _FURL_FEATURES_H_
#define _FURL_FEATURES_H_

#include <furl/furl.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* int is better since we can know if we have those features
   and store the position */
typedef struct _furl_features_t furl_features_t;
struct _furl_features_t {
	int scheme;
	int hierarchical;
	int credential;
	int domain;
	int port;
	int resource_path;
	int query_string;
	int fragment;
};

furl_features_t furl_features_init(void);
/**
 * This function will find positions for big level features of an url: domain, credentials, etc.
 */
furl_features_t furl_features_find(furl_handler_t *fh, char *url, size_t url_len);
void furl_features_debug(char *url, furl_features_t features);
int furl_features_exist(int feature);
int furl_features_errors_lookup(furl_features_t url_features);

#endif	/* _FURL_FEATURES_H_ */
