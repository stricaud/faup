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
typedef struct _furl_handler_t furl_handler_t;
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

furl_handler_t *furl_init(void);
char *furl_get_version(void);
void furl_show(furl_handler_t const* fh, const char sep_char, FILE* out);
void furl_terminate(furl_handler_t *fh);

#endif	/* _FURL_FURL_H_ */
