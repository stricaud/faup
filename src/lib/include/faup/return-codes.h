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

#ifndef _FAUP_RETURN_CODES_H_
#define _FAUP_RETURN_CODES_H_

#define FAUP_OK 0

#define FAUP_INVALID_STRONG 100	/* The string cannot be parsed at all */
#define FAUP_INVALID_MEDIUM 101	/* The string sucks, but we can still get some stuff (eg. 'http://@') */
#define FAUP_INVALID_WEAK 102 	/* The string can be mostly parsed, except a few weird stuff (eg. 'localhost') */


#endif	/* _FAUP_RETURN_CODES_H_ */
