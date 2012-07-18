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

	fh = malloc(sizeof(furl_handler_t));

	return fh;
}

void furl_show(furl_handler_t const* fh, const char sep_char, FILE* out)
{
	// Output order is:
	// scheme,credential,domain,tld,port,resource_path,query_string,fragment
	furl_features_show(fh, fh->furl.features.scheme, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.credential, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.domain, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.tld, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.port, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.resource_path, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.query_string, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.fragment, out);
	fwrite(&sep_char, 1, 1, out);
	//furl_features_show(fh, fh->furl.features.hierarchical, out);
	//fwrite(&sep_char, 1, 1, out);
}

void furl_terminate(furl_handler_t *fh)
{
	free(fh);
}
