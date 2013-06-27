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

#ifndef _FAUP_FAUP_H_
#define _FAUP_FAUP_H_

#include <faup/errors.h>
#include <faup/features.h>
#include <faup/handler.h>
#include <faup/portable.h>
#include <faup/tld.h>
#include <faup/version.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef WIN32
 	#ifndef bool
 		#define bool int
 	#endif // bool
	#ifndef true
		#define true 1
	#endif // true
	#ifndef false
 		#define false 0
 	#endif // false
#else
 	#include <stdbool.h>
#endif // WIN32

#ifdef WIN32
	#include <windows.h>
#endif

#ifdef WIN32
	#define ssize_t SSIZE_T
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FAUP_MAXLEN 8192
#define FAUP_MAXPATHLEN 1024 /* Because MAXPATHLEN, MAX_PATH, PATH_MAX are all different 'standards' (ahah!) */

#define FAUP_OS_DIRSEP "/"


typedef struct _faup_t faup_t;
typedef enum _faup_last_slash_t faup_last_slash_t;

struct _faup_t {
	const char *org_str;
	faup_features_t features;
};

struct _faup_handler_t {
	faup_t faup;
};

enum _faup_last_slash_t {
	FAUP_LAST_SLASH_NOTFOUND,
	FAUP_LAST_SLASH_HIERARCHICAL,
	FAUP_LAST_SLASH_AFTER_DOMAIN,
};

#define faup_get_pos(fh, name) (fh)->faup.features.name.pos
#define faup_get_size(fh, name) (fh)->faup.features.name.size

faup_handler_t *faup_init(void);
char *faup_get_version(void);
void faup_terminate(faup_handler_t *fh);

int32_t faup_get_scheme_pos(faup_handler_t *fh);
uint32_t faup_get_scheme_size(faup_handler_t *fh);
int32_t faup_get_credential_pos(faup_handler_t *fh);
uint32_t faup_get_credential_size(faup_handler_t *fh);
int32_t faup_get_subdomain_pos(faup_handler_t *fh);
uint32_t faup_get_subdomain_size(faup_handler_t *fh);
int32_t faup_get_domain_pos(faup_handler_t *fh);
uint32_t faup_get_domain_size(faup_handler_t *fh);
int32_t faup_get_domain_without_tld_pos(faup_handler_t *fh);
uint32_t faup_get_domain_without_tld_size(faup_handler_t *fh);
int32_t faup_get_host_pos(faup_handler_t *fh);
uint32_t faup_get_host_size(faup_handler_t *fh);
int32_t faup_get_tld_pos(faup_handler_t *fh);
uint32_t faup_get_tld_size(faup_handler_t *fh);
int32_t faup_get_port_pos(faup_handler_t *fh);
uint32_t faup_get_port_size(faup_handler_t *fh);
int32_t faup_get_resource_path_pos(faup_handler_t *fh);
uint32_t faup_get_resource_path_size(faup_handler_t *fh);
int32_t faup_get_query_string_pos(faup_handler_t *fh);
uint32_t faup_get_query_string_size(faup_handler_t *fh);
int32_t faup_get_fragment_pos(faup_handler_t *fh);
uint32_t faup_get_fragment_size(faup_handler_t *fh);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_FAUP_H_ */
