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

#ifndef _FAUP_OPTIONS_H_
#define _FAUP_OPTIONS_H_

#include <stdio.h>
#include <faup/faup.h>
#include <faup/tld-tree.h>

#ifdef __cplusplus
extern "C" {
#endif

enum _faup_output_t {
	FAUP_OUTPUT_CSV,
	FAUP_OUTPUT_JSON,
};
typedef enum _faup_output_t faup_output_t;

enum _faup_url_field_t {
	FAUP_URL_FIELD_NONE          = (1 << 0),
	FAUP_URL_FIELD_LINE          = (1 << 1),
	FAUP_URL_FIELD_SCHEME        = (1 << 2),
	FAUP_URL_FIELD_CREDENTIAL    = (1 << 3),
	FAUP_URL_FIELD_SUBDOMAIN     = (1 << 4),
	FAUP_URL_FIELD_DOMAIN        = (1 << 5),
	FAUP_URL_FIELD_HOST          = (1 << 6),
	FAUP_URL_FIELD_TLD           = (1 << 7),
	FAUP_URL_FIELD_PORT          = (1 << 8),
	FAUP_URL_FIELD_RESOURCE_PATH = (1 << 9),
	FAUP_URL_FIELD_QUERY_STRING  = (1 << 10),
	FAUP_URL_FIELD_FRAGMENT      = (1 << 11),
};
typedef enum _faup_url_field_t faup_url_field_t;

enum _faup_input_source_t {
	FAUP_INPUT_SOURCE_ARGUMENT,
	FAUP_INPUT_SOURCE_PIPE,
	FAUP_INPUT_SOURCE_FILE,
};
typedef enum _faup_input_source_t faup_input_source_t;

struct _faup_options_t {
  int print_header;
  int print_line;
  long current_line;
  char sep_char;
  faup_input_source_t input_source;
  faup_output_t output;
  void *tld_tree; /* TLDNode */
  int tld_greater_extraction;
  faup_url_field_t fields;
};
typedef struct _faup_options_t faup_options_t;

void faup_options_defaults(faup_options_t *opts);
void faup_options_debug(faup_options_t *opts);
int faup_options_url_field_has_greater_than(faup_options_t *opts, faup_url_field_t field);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_OPTIONS_H_ */

