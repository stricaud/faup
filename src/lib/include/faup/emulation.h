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

#ifndef _FAUP_EMULATION_H_
#define _FAUP_EMULATION_H_

#include <faup/faup.h>
#include <faup/features.h>
#include <faup/options.h>

#ifdef __cplusplus
extern "C" {
#endif

char *faup_emulation_internet_explorer(faup_handler_t const* fh, const faup_feature_t feature);
char *faup_emulation_safari(faup_handler_t const* fh, const faup_feature_t feature);
char *faup_emulation_firefox(faup_handler_t const* fh, const faup_feature_t feature);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_EMULATION_H_ */
