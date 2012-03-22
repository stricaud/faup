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

#include <furl/furl.h>

#include <stdlib.h>

furl_handler_t *furl_init(void)
{
	furl_handler_t *fh;

	fh = malloc(sizeof(furl_handler_t *));
	fh->allocated_buf = malloc(FURL_MAXLEN);
	fh->sep_char = ',';

	return fh;
}

void furl_terminate(furl_handler_t *fh)
{
	free(fh->allocated_buf);
	free(fh);
}
