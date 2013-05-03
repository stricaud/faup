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

/* For the moment, this file is more a memo than something used in
 * faup. 
 *
 * Reference in case we need it: http://www.boutell.com/newfaq/misc/urllength.html
 */

#ifndef _FAUP_URLLENGTHS_H_
#define _FAUP_URLLENGTHS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * From RFC 2616 (section 3.2.1):
 * The HTTP protocol does not place any a priori limit on the length of a URI. 
 * Servers MUST be able to handle the URI of any resource they serve, and SHOULD be able to handle URIs of unbounded 
 * length if they provide GET-based forms that could generate such URIs. A server SHOULD return 414 (Request-URI Too Long) 
 * status if a URI is longer than the server can handle 
 */

/*
 * That said, browsers and servers have some limits. The idea here is to list all of them in case we need it one day
 */
 #define IE_URL_MAXLEN 2083
 #define APACHE_URL_MAXLEN 8192
 #define IIS_URL_MAXLEN 16384

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_URLLENGTHS_H_ */

