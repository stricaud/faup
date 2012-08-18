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

char *furl_get_version(void)
{
  return FURL_VERSION;
}

void furl_show(furl_handler_t const* fh, const char sep_char, FILE* out)
{
	// Output order is:
	// scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	furl_features_show(fh, fh->furl.features.scheme, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.credential, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.subdomain, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.domain, out);
	fwrite(&sep_char, 1, 1, out);
	furl_features_show(fh, fh->furl.features.host, out);
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
}

void furl_terminate(furl_handler_t *fh)
{
	free(fh);
}

int32_t furl_get_scheme_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, scheme);
}

uint32_t furl_get_scheme_size(furl_handler_t *fh)
{
        return furl_get_size(fh, scheme);
}

int32_t furl_get_credential_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, credential);
}

uint32_t furl_get_credential_size(furl_handler_t *fh)
{
        return furl_get_size(fh, credential);
}

int32_t furl_get_subdomain_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, subdomain);
}

uint32_t furl_get_subdomain_size(furl_handler_t *fh)
{
        return furl_get_size(fh, subdomain);
}

int32_t furl_get_domain_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, domain);
}

uint32_t furl_get_domain_size(furl_handler_t *fh)
{
        return furl_get_size(fh, domain);
}

int32_t furl_get_host_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, host);
}

uint32_t furl_get_host_size(furl_handler_t *fh)
{
        return furl_get_size(fh, host);
}

int32_t furl_get_tld_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, tld);
}

uint32_t furl_get_tld_size(furl_handler_t *fh)
{
        return furl_get_size(fh, tld);
}

int32_t furl_get_port_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, port);
}

uint32_t furl_get_port_size(furl_handler_t *fh)
{
        return furl_get_size(fh, port);
}

int32_t furl_get_resource_path_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, resource_path);
}

uint32_t furl_get_resource_path_size(furl_handler_t *fh)
{
        return furl_get_size(fh, resource_path);
}

int32_t furl_get_query_string_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, query_string);
}

uint32_t furl_get_query_string_size(furl_handler_t *fh)
{
        return furl_get_size(fh, query_string);
}

int32_t furl_get_fragment_pos(furl_handler_t *fh)
{
        return furl_get_pos(fh, fragment);
}

uint32_t furl_get_fragment_size(furl_handler_t *fh)
{
        return furl_get_size(fh, fragment);
}








