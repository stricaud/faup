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

#ifdef __cplusplus
extern "C" {
#endif

void faup_output_csv(faup_handler_t const* fh, const char sep_char, FILE* out);
void faup_output_json(faup_handler_t const* fh, FILE* out);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_OUTPUT_H_ */

