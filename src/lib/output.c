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
#include <faup/features.h>
#include <faup/output.h>

#ifdef FAUP_LUA_MODULES
#include <faup/modules.h>
#endif

void faup_output_show(faup_handler_t const* fh, faup_options_t *opts, const faup_feature_t feature, FILE* out, int escape_dquotes)
{

	if (faup_features_exist(feature)) {
		uint32_t counter = 0;
		const char *tmpbuf = NULL;
		tmpbuf = &fh->faup.org_str[feature.pos];

		if (feature.size > FAUP_MAXLEN) {
			fprintf(stderr, "Error: field(%s) with size(%d) is greater than the maximum default FAUP URL MAXLEN (%d). Cannot process the given url (%s).\n",
					faup_features_get_field_name(feature), feature.size, FAUP_MAXLEN, fh->faup.org_str);
			return;
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

void faup_output_csv_header(faup_handler_t const* fh, faup_options_t *opts, FILE *out) 
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

// If that function is called more than once, you are doing a bad thing.
char *faup_output_json_buffer_allocate(void)
{
	char *allocated_buffer;

	// domain_without_tld = 18, max potential key, includes extra {} and "", this is not 100% accurate but still allows to
	// handle the worst case and avoid multiple allocations; 
	// It is done two times because urls can be made if " that will be escaped. Preventing the worst cases situations.
	// And since this is only one alloc, that does not hurt much.
	allocated_buffer = calloc(2 * FAUP_FEATURES_NUMBER, (FAUP_MAXLEN + 18));
	if (!allocated_buffer) {
		fprintf(stderr, "Cannot allocate buffer for %s\n", __FUNCTION__);
	}

	return allocated_buffer;
}

void faup_output_buffer_append(char *buffer, int *buffer_pos, const char *data, size_t data_size)
{
	memcpy(&buffer[*buffer_pos], data, data_size);
	*buffer_pos += data_size;
	buffer[*buffer_pos] = '\0';
}

void faup_output_show_buffer(faup_handler_t const* fh, faup_options_t *opts, const faup_feature_t feature, char *buffer, int *buffer_pos, int escape_dquotes)
{

	if (faup_features_exist(feature)) {
		uint32_t counter = 0;
		const char *tmpbuf = NULL;
		tmpbuf = &fh->faup.org_str[feature.pos];

		if (feature.size > FAUP_MAXLEN) {
			fprintf(stderr, "Error: field(%s) with size(%d) is greater than the maximum default FAUP URL MAXLEN (%d). Cannot process the given url (%s).\n",
					faup_features_get_field_name(feature), feature.size, FAUP_MAXLEN, fh->faup.org_str);
			return;
		}

		while (counter < feature.size) {
			if (escape_dquotes) {
				if (tmpbuf[counter] == '"') {
					faup_output_buffer_append(buffer, buffer_pos, "\\", 1);
				}
			}

			// printf("tmpbuf:%c\n", tmpbuf[counter]);
			faup_output_buffer_append(buffer, buffer_pos, &fh->faup.org_str[feature.pos+counter], 1);

			counter++;
		}

	}
}

void _faup_output_json_single_buffer(faup_handler_t const* fh, faup_options_t *opts, char *faup_feature_name, const faup_feature_t feature, faup_url_field_t field, char *buffer, int *buffer_pos)
{

	faup_output_buffer_append(buffer, buffer_pos, "\t\"", 2);
	faup_output_buffer_append(buffer, buffer_pos, faup_feature_name, strlen(faup_feature_name));
	faup_output_buffer_append(buffer, buffer_pos, "\": \"", 4);

	faup_output_show_buffer(fh, opts, feature, buffer, buffer_pos, 1);

	if (faup_options_url_field_has_greater_than(opts, field)) {
		faup_output_buffer_append(buffer, buffer_pos, "\",\n", 3);
	} else {
		faup_output_buffer_append(buffer, buffer_pos, "\"", 1);
	}

}

// Requires an allocated buffer. 
void faup_output_json_buffer(faup_handler_t const* fh, faup_options_t *opts, char *buffer)
{
	int buffer_pos = 0;

	if (!fh->faup.decoded) {
		fprintf(stderr, "Error: Cannot output json, since the URL has not been decoded. Call faup_decode()!\n");
		return;
	}

	faup_output_buffer_append(buffer, &buffer_pos, "{\n", 2);

	if (opts->fields & FAUP_URL_FIELD_SCHEME) {
		_faup_output_json_single_buffer(fh, opts, "scheme", fh->faup.features.scheme, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}	
	if (opts->fields & FAUP_URL_FIELD_CREDENTIAL) {
		_faup_output_json_single_buffer(fh, opts, "credential", fh->faup.features.credential, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_SUBDOMAIN) {
		_faup_output_json_single_buffer(fh, opts, "subdomain", fh->faup.features.subdomain, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_DOMAIN) {
		_faup_output_json_single_buffer(fh, opts, "domain", fh->faup.features.domain, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD) {
		_faup_output_json_single_buffer(fh, opts, "domain_without_tld", fh->faup.features.domain_without_tld, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_HOST) {
		_faup_output_json_single_buffer(fh, opts, "host", fh->faup.features.host, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_TLD) {
		_faup_output_json_single_buffer(fh, opts, "tld", fh->faup.features.tld, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_PORT) {
		_faup_output_json_single_buffer(fh, opts, "port", fh->faup.features.port, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_RESOURCE_PATH) {
		_faup_output_json_single_buffer(fh, opts, "resource_path", fh->faup.features.resource_path, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_QUERY_STRING) {
		_faup_output_json_single_buffer(fh, opts, "query_string", fh->faup.features.query_string, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}
	if (opts->fields & FAUP_URL_FIELD_FRAGMENT) {
		_faup_output_json_single_buffer(fh, opts, "fragment", fh->faup.features.fragment, FAUP_URL_FIELD_SCHEME, 
										buffer, &buffer_pos);
	}

	faup_output_buffer_append(buffer, &buffer_pos, "\n}\n", 3);
		
}

void faup_output_json(faup_handler_t const* fh, faup_options_t *opts, FILE* out)
{
	if (!fh->faup.decoded) {
		fprintf(stderr, "Error: Cannot output json, since the URL has not been decoded. Call faup_decode()!\n");
		return;
	}

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

void faup_output(faup_handler_t *fh, FILE* out)
{
#ifdef FAUP_LUA_MODULES
	bool module_executed;

	module_executed = faup_modules_url_output(fh, out);
#endif

	// faup_features_debug("output", &fh->faup.features);

	switch(fh->options->output) {
		case FAUP_OUTPUT_CSV:
			faup_output_csv_header(fh, fh->options, out);

			if (fh->options->print_line) {
					switch(fh->options->input_source) {
						case FAUP_INPUT_SOURCE_ARGUMENT:
							printf("0%c", fh->options->sep_char);
							break;
						case FAUP_INPUT_SOURCE_PIPE:
						case FAUP_INPUT_SOURCE_FILE:
							printf("%ld%c", fh->options->current_line, fh->options->sep_char);
							break;
					}
			}
			faup_output_csv(fh, fh->options, out);
			break;
		case FAUP_OUTPUT_JSON:
			faup_output_json(fh, fh->options, out);
			break;
		case FAUP_OUTPUT_MODULE:
			// We don't do a thing since our modules did 
			break;
		default:
			fprintf(stderr, "Error: unknown output option %d\n", fh->options->output);
	}
}
