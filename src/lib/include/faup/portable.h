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

#ifndef _FAUP_PORTABLE_H_
#define _FAUP_PORTABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MACOS
void *memrchr(const void *s, int c, size_t n);
#endif /* MACOS */


#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_PORTABLE_H_ */
