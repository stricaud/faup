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

#ifndef _FAUP_TLD_H_
#define _FAUP_TLD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SOCKBUF_LINE 4096

#define MOZILLA_TLD_LIST_DOMAIN "mxr.mozilla.org"
#define MOZILLA_TLD_LIST_IP "63.245.215.42"
#define MOZILLA_TLD_LIST_PORT 80
#define MOZILLA_TLD_LIST_URL "http://mxr.mozilla.org/mozilla-central/source/netwerk/dns/effective_tld_names.dat?raw=1"
#define MOZILLA_TLD_LIST_GET "GET " MOZILLA_TLD_LIST_URL " HTTP/1.1\n\n"

int faup_tld_get_mozilla_list(char *store_to_file);

/* Returns allocated strings */
char *faup_tld_get_file_from_home(char *append);
char *faup_tld_get_file(char *append);
char *faup_tld_file_to_write(void);
int faup_tld_update(void);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_TLD_H_ */

