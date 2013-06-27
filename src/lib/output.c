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
#include <stdlib.h>

#include <faup/faup.h>
#include <faup/output.h>
#include <faup/emulation.h>

void faup_output_show(faup_handler_t const* fh, faup_options_t *opts, const faup_feature_t feature, FILE* out, int escape_dquotes)
{

	if (faup_features_exist(feature)) {
		char *emulation_str = NULL;
		uint32_t counter = 0;
		const char *tmpbuf = NULL;
		tmpbuf = &fh->faup.org_str[feature.pos];

		switch (opts->emulation) {
			case FAUP_BROWSER_EMULATION_IE:
				emulation_str = faup_emulation_internet_explorer(fh, feature);
				if (emulation_str) {
					fprintf(out, "%s", emulation_str);
					free(emulation_str);
					return;
				} 
			break;
			case FAUP_BROWSER_EMULATION_SAFARI:
				emulation_str = faup_emulation_safari(fh, feature);
				if (emulation_str) {
					fprintf(out, "%s", emulation_str);
					free(emulation_str);
					return;
				} 
			break;
			case FAUP_BROWSER_EMULATION_FIREFOX:
				emulation_str = faup_emulation_firefox(fh, feature);
				if (emulation_str) {
					fprintf(out, "%s", emulation_str);
					free(emulation_str);
					return;
				} 
			break;
			case FAUP_BROWSER_EMULATION_NONE:
			default:
				break;
		}

		while (counter < feature.size) {

			if (escape_dquotes) {
				if (tmpbuf[counter] == '"') {
					fwrite(&"\\", 1, 1, out);
				}
			}

			fwrite(&tmpbuf[counter], 1, 1, out);
			counter++;

		}


	}
}

void _faup_output_csv_single(faup_handler_t const* fh, faup_options_t *opts, FILE *out, faup_url_field_t field)
{


		if (opts->fields & field) {
			faup_output_show(fh, opts, faup_options_field_get_feature(fh, field), out, 0);
			if (faup_options_url_field_has_greater_than(opts, field)) {
				fwrite(&opts->sep_char, 1, 1, out);
			} else {
				fwrite("\n", 1, 1, out);
			}
	}
}

void _faup_output_csv_header_single(faup_options_t *opts, FILE *out, faup_url_field_t field, char *string)
{
		if (opts->fields & field) {
		fprintf(out, "%s", string);
		if (faup_options_url_field_has_greater_than(opts, field)) {
			fwrite(&opts->sep_char, 1, 1, out);
		} else {
			fwrite("\n", 1, 1, out);
		}
	}
}

void faup_output_csv_header(faup_handler_t const* fh, faup_options_t *opts, FILE *out) \
{
	if (!opts->print_header) return;

	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_LINE, "line");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_SCHEME, "scheme");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_CREDENTIAL, "credential");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_SUBDOMAIN, "subdomain");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_DOMAIN, "domain");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD, "domain_without_tld");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_HOST, "host");	
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_TLD, "tld");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_PORT, "port");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_RESOURCE_PATH, "resource_path");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_QUERY_STRING, "query_string");
	_faup_output_csv_header_single(opts, out, FAUP_URL_FIELD_FRAGMENT, "fragment");
}

void faup_output_csv(faup_handler_t const* fh, faup_options_t *opts, FILE* out)
{

	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_SCHEME);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_CREDENTIAL);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_SUBDOMAIN);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_DOMAIN);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_HOST);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_TLD);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_PORT);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_RESOURCE_PATH);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_QUERY_STRING);
	_faup_output_csv_single(fh, opts, out, FAUP_URL_FIELD_FRAGMENT);
}

void _faup_output_json_single(faup_handler_t const* fh, faup_options_t *opts, char *faup_feature_name, const faup_feature_t feature, FILE *out, faup_url_field_t field)
{

	fwrite(&"\t\"", 2, 1, out);
	fwrite(faup_feature_name, strlen(faup_feature_name), 1, out);
	fwrite("\": \"", 4, 1, out);

	faup_output_show(fh, opts, feature, out, 1);

	if (faup_options_url_field_has_greater_than(opts, field)) {
		fwrite("\",\n", 3, 1, out);
	} else {
		fwrite("\"", 1, 1, out);
	}

}

void faup_output_json(faup_handler_t const* fh, faup_options_t *opts, FILE* out)
{	

	fwrite(&"{\n", 2, 1, out);
	if (opts->fields & FAUP_URL_FIELD_SCHEME) {
		_faup_output_json_single(fh, opts, "scheme", fh->faup.features.scheme, out, FAUP_URL_FIELD_SCHEME);
	}
	if (opts->fields & FAUP_URL_FIELD_CREDENTIAL) {	
		_faup_output_json_single(fh, opts, "credential", fh->faup.features.credential, out, FAUP_URL_FIELD_CREDENTIAL);
	}
	if (opts->fields & FAUP_URL_FIELD_SUBDOMAIN) {
		_faup_output_json_single(fh, opts, "subdomain", fh->faup.features.subdomain, out, FAUP_URL_FIELD_SUBDOMAIN);
	}
	if (opts->fields & FAUP_URL_FIELD_DOMAIN) {
		_faup_output_json_single(fh, opts, "domain", fh->faup.features.domain, out, FAUP_URL_FIELD_DOMAIN);
	}
	if (opts->fields & FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD) {
		_faup_output_json_single(fh, opts, "domain_without_tld", fh->faup.features.domain_without_tld, out, FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD);
	}
	if (opts->fields & FAUP_URL_FIELD_HOST) {
		_faup_output_json_single(fh, opts, "host", fh->faup.features.host, out, FAUP_URL_FIELD_HOST);
	}
	if (opts->fields & FAUP_URL_FIELD_TLD) {	
		_faup_output_json_single(fh, opts, "tld", fh->faup.features.tld, out, FAUP_URL_FIELD_TLD);
	}
	if (opts->fields & FAUP_URL_FIELD_PORT) {
		_faup_output_json_single(fh, opts, "port", fh->faup.features.port, out, FAUP_URL_FIELD_PORT);
	}
	if (opts->fields & FAUP_URL_FIELD_RESOURCE_PATH) {
		_faup_output_json_single(fh, opts, "resource_path", fh->faup.features.resource_path, out, FAUP_URL_FIELD_RESOURCE_PATH);
	}
	if (opts->fields & FAUP_URL_FIELD_QUERY_STRING) {
		_faup_output_json_single(fh, opts, "query_string", fh->faup.features.query_string, out, FAUP_URL_FIELD_QUERY_STRING);
	}
	if (opts->fields & FAUP_URL_FIELD_FRAGMENT) {
		_faup_output_json_single(fh, opts, "fragment", fh->faup.features.fragment, out, FAUP_URL_FIELD_FRAGMENT);
	}


	fwrite(&"\n}\n", 3, 1, out);
}

void faup_output(faup_handler_t const* fh, faup_options_t *opts, FILE* out)
{
	switch(opts->output) {
		case FAUP_OUTPUT_CSV:
			faup_output_csv_header(fh, opts, out);

			if (opts->print_line) {
					switch(opts->input_source) {
						case FAUP_INPUT_SOURCE_ARGUMENT:
							printf("0%c", opts->sep_char);
							break;
						case FAUP_INPUT_SOURCE_PIPE:
						case FAUP_INPUT_SOURCE_FILE:
							printf("%ld%c", opts->current_line, opts->sep_char);
							break;
					}
			}
			faup_output_csv(fh, opts, out);
			break;
		case FAUP_OUTPUT_JSON:
			faup_output_json(fh, opts, out);
			break;
		default:
			fprintf(stderr, "Error: unknown output option %d\n", opts->output);
	}
}
