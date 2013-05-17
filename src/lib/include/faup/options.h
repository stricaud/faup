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

enum _faup_output_t {
	FAUP_OUTPUT_CSV,
	FAUP_OUTPUT_JSON,
};
typedef enum _faup_output_t faup_output_t;

struct _faup_options_t {
  int print_header;
  int print_line;
  char sep_char;
  faup_output_t output;
  TLDNode * tld_tree;
  int tld_greater_extraction;
};
typedef struct _faup_options_t faup_options_t;

#ifdef __cplusplus
extern "C" {
#endif

void faup_options_defaults(faup_options_t *opts);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_OPTIONS_H_ */

