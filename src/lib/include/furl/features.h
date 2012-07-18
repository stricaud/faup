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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _furl_handler_t furl_handler_t;

typedef struct _furl_feature_t furl_feature_t;
// This will be aligned on 8 bytes on 64-bit platforms, that's
// why 'size' is not of type size_t.
struct _furl_feature_t {
	int32_t pos;
	uint32_t size;
};

/* int is better since we can know if we have those features
   and store the position */
typedef struct _furl_features_t furl_features_t;
struct _furl_features_t {
	furl_feature_t scheme;
	furl_feature_t hierarchical;
	furl_feature_t credential;
	furl_feature_t host;
	furl_feature_t domain;
	furl_feature_t subdomain;
	furl_feature_t tld;
	furl_feature_t port;
	furl_feature_t resource_path;
	furl_feature_t query_string;
	furl_feature_t fragment;
};

void furl_features_init(furl_features_t* features);
/**
 * This function will find positions for big level features of an url: domain, credentials, etc.
 */
void furl_features_find(furl_handler_t *fh, const char *url, const size_t url_len);
void furl_features_debug(const char *url, furl_features_t const* features);
int furl_features_exist(furl_feature_t feature);
void furl_features_show(furl_handler_t const* fh, const furl_feature_t feature, FILE* out);
int furl_features_errors_lookup(furl_features_t const* url_features);

#endif	/* _FURL_FEATURES_H_ */
