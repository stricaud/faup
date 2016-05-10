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

#ifndef _FAUP_OUTPUT_H_
#define _FAUP_OUTPUT_H_

#include <stdio.h>
#include <faup/faup.h>
#include <faup/options.h>

#ifdef __cplusplus
extern "C" {
#endif

// domain_without_tld = 18, max potential key, includes extra {} and "", this is not 100% accurate but still allows to
// handle the worst case and avoid multiple allocations; 
// It is done two times because urls can be made if " that will be escaped. Preventing the worst cases situations.
// And since this is only one alloc, that does not hurt much.
#define FAUP_MAX_JSON_BUFFER_SIZE (2 * FAUP_FEATURES_NUMBER) * (FAUP_MAXLEN + 18)

void faup_output(faup_handler_t *fh, FILE* out);
void faup_output_csv_header(faup_handler_t const* fh, faup_options_t *opts, FILE *out);
void faup_output_csv(faup_handler_t const* fh, faup_options_t *opts, FILE* out);
void faup_output_json(faup_handler_t const* fh, faup_options_t *opts, FILE* out);
char *faup_output_json_buffer_allocate(void);
void faup_output_json_buffer(faup_handler_t const* fh, faup_options_t *opts, char *buffer);
char *faup_output_get_string_from_url_type(faup_handler_t const* fh);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_OUTPUT_H_ */

