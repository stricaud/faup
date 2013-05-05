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

#include <stdio.h>

#include <faup/faup.h>
#include <faup/output.h>

void faup_output_show(faup_handler_t const* fh, const faup_feature_t feature, FILE* out)
{
	if (faup_features_exist(feature)) {
		fwrite(fh->faup.org_str + feature.pos, feature.size, 1, out);
	}
}

void faup_output_csv(faup_handler_t const* fh, const char sep_char, FILE* out)
{
	// Output order is:
	// scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	faup_output_show(fh, fh->faup.features.scheme, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.credential, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.subdomain, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.domain, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.host, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.tld, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.port, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.resource_path, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.query_string, out);
	fwrite(&sep_char, 1, 1, out);
	faup_output_show(fh, fh->faup.features.fragment, out);
}

void _faup_output_json_single(faup_handler_t const* fh, char *faup_feature_name, const faup_feature_t feature, FILE *out)
{
	const char *tmpbuf = NULL;
	size_t buflen = 0;
	size_t counter = 0;

	fwrite(&"\t\"", 2, 1, out);
	fwrite(faup_feature_name, strlen(faup_feature_name), 1, out);
	fwrite("\": \"", 4, 1, out);
	tmpbuf = &fh->faup.org_str[feature.pos];
	buflen = feature.size;
	while (counter < buflen) {
		if (tmpbuf[counter] == '"') {
			fwrite(&"\\", 1, 1, out);
		}
		fwrite(&tmpbuf[counter], 1, 1, out);
		counter++;
	}
	fwrite("\",\n", 3, 1, out);
}

void faup_output_json(faup_handler_t const* fh, FILE* out)
{	

	fwrite(&"{\n", 2, 1, out);
	_faup_output_json_single(fh, "scheme", fh->faup.features.scheme, out);
	_faup_output_json_single(fh, "credential", fh->faup.features.credential, out);
	_faup_output_json_single(fh, "subdomain", fh->faup.features.subdomain, out);
	_faup_output_json_single(fh, "domain", fh->faup.features.domain, out);
	_faup_output_json_single(fh, "host", fh->faup.features.host, out);
	_faup_output_json_single(fh, "tld", fh->faup.features.tld, out);
	_faup_output_json_single(fh, "port", fh->faup.features.port, out);
	_faup_output_json_single(fh, "resource_path", fh->faup.features.resource_path, out);
	_faup_output_json_single(fh, "query_string", fh->faup.features.query_string, out);
	_faup_output_json_single(fh, "fragment", fh->faup.features.fragment, out);


	fwrite(&"\n}\n", 3, 1, out);
}
