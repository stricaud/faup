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

#define MAX_RECVBUF 4096

  /*
   * Originally, the list was on http://mxr.mozilla.org/mozilla-central/source/netwerk/dns/effective_tld_names.dat?raw=1
   * However, it recently moved to https://publicsuffix.org/list/effective_tld_names.dat
   * and I don't want to handle SSL in this simple stupid code. So I move to my own server that stuff that I mirror and 
   * we are good to go.
   */
#define MOZILLA_TLD_LIST_DOMAIN "io.libio.so"
#define MOZILLA_TLD_LIST_IP "37.187.73.159"
#define MOZILLA_TLD_LIST_PORT 80
#define MOZILLA_TLD_LIST_URL "http://io.libio.so/effective_tld_names.dat"

#define MOZILLA_TLD_LIST_GET "GET " MOZILLA_TLD_LIST_URL " HTTP/1.0\n\n"

int faup_tld_download_mozilla_list(char *store_to_file);

/* Returns allocated strings */
char *faup_tld_get_file_from_home(char *append);
char *faup_tld_get_file(char *append);
char *faup_tld_file_to_write(void);
int faup_tld_update(void);

int faup_tld_array_populate(void);
void faup_tld_array_destroy(void);
void faup_tld_array_cb_to_stdout(char *tld, void *user_data);
void faup_tld_array_foreach(void (*cb_tld_array)(char *tld, void *user_data), void *user_data);
void faup_tld_datadir_print(void);
char *faup_tld_get_filename(void);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_TLD_H_ */

