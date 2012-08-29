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

#ifndef _FURL_FURL_H_
#define _FURL_FURL_H_

#include <furl/errors.h>
#include <furl/features.h>
#include <furl/handler.h>
#include <furl/portable.h>
#include <furl/version.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FURL_MAXLEN 8192

typedef struct _furl_t furl_t;
typedef enum _furl_last_slash_t furl_last_slash_t;

struct _furl_t {
	const char *org_str;
	furl_features_t features;
};

struct _furl_handler_t {
	furl_t furl;
};

enum _furl_last_slash_t {
	FURL_LAST_SLASH_NOTFOUND,
	FURL_LAST_SLASH_HIERARCHICAL,
	FURL_LAST_SLASH_AFTER_DOMAIN,
};

#define furl_get_pos(fh, name) (fh)->furl.features.name.pos
#define furl_get_size(fh, name) (fh)->furl.features.name.size

furl_handler_t *furl_init(void);
char *furl_get_version(void);
void furl_show(furl_handler_t const* fh, const char sep_char, FILE* out);
void furl_terminate(furl_handler_t *fh);

int32_t furl_get_scheme_pos(furl_handler_t *fh);
uint32_t furl_get_scheme_size(furl_handler_t *fh);
int32_t furl_get_credential_pos(furl_handler_t *fh);
uint32_t furl_get_credential_size(furl_handler_t *fh);
int32_t furl_get_subdomain_pos(furl_handler_t *fh);
uint32_t furl_get_subdomain_size(furl_handler_t *fh);
int32_t furl_get_domain_pos(furl_handler_t *fh);
uint32_t furl_get_domain_size(furl_handler_t *fh);
int32_t furl_get_host_pos(furl_handler_t *fh);
uint32_t furl_get_host_size(furl_handler_t *fh);
int32_t furl_get_tld_pos(furl_handler_t *fh);
uint32_t furl_get_tld_size(furl_handler_t *fh);
int32_t furl_get_port_pos(furl_handler_t *fh);
uint32_t furl_get_port_size(furl_handler_t *fh);
int32_t furl_get_resource_path_pos(furl_handler_t *fh);
uint32_t furl_get_resource_path_size(furl_handler_t *fh);
int32_t furl_get_query_string_pos(furl_handler_t *fh);
uint32_t furl_get_query_string_size(furl_handler_t *fh);
int32_t furl_get_fragment_pos(furl_handler_t *fh);
uint32_t furl_get_fragment_size(furl_handler_t *fh);

#ifdef __cplusplus
}
#endif

#endif	/* _FURL_FURL_H_ */
