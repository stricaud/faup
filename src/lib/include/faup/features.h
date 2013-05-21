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

#ifndef _FAUP_FEATURES_H_
#define _FAUP_FEATURES_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <faup/faup.h>
#include <faup/handler.h>
#include <faup/options.h>

#ifdef __cplusplus
extern "C" {
#endif

// This will be aligned on 8 bytes on 64-bit platforms, that's
// why 'size' is not of type size_t.
struct _faup_feature_t {
	int32_t pos;
	uint32_t size;
};
typedef struct _faup_feature_t faup_feature_t;

/* int is better since we can know if we have those features
   and store the position */
struct _faup_features_t {
	faup_feature_t scheme;
	faup_feature_t hierarchical;
	faup_feature_t credential;
	faup_feature_t host;
	faup_feature_t domain;
	faup_feature_t subdomain;
	faup_feature_t tld;
	faup_feature_t port;
	faup_feature_t resource_path;
	faup_feature_t query_string;
	faup_feature_t fragment;
};
typedef struct _faup_features_t faup_features_t;

void faup_features_init(faup_features_t* features);
/**
 * This function will find positions for big level features of an url: domain, credentials, etc.
 */
void faup_features_find(faup_handler_t *fh, const char *url, const size_t url_len);
void faup_features_debug(const char *url, faup_features_t const* features);
int faup_features_exist(faup_feature_t feature);
void faup_features_show(faup_handler_t const* fh, const faup_feature_t feature, FILE* out);
int faup_features_errors_lookup(faup_features_t const* url_features);
//faup_feature_t faup_features_get_from_field(faup_handler_t *fh, faup_url_field_t field);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_FEATURES_H_ */
