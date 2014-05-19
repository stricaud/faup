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

#ifndef _FAUP_WEBSERVER_H_
#define _FAUP_WEBSERVER_H_

#include <faup/faup.h>
#include <faup/civetweb/civetweb.h>

#ifdef __cplusplus
extern "C" {
#endif

int faup_webserver_start(faup_handler_t *fh, faup_options_t *faup_opts, char *listening_ports);
int faup_webserver_stop();

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_WEBSERVER_H_ */
