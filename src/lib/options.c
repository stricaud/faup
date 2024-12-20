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

#include <stdlib.h>

#include <faup/tld-tree.h>
#include <faup/options.h>

faup_options_t *faup_options_new(void)
{
	faup_options_t *opts;

	opts = malloc(sizeof(faup_options_t));
	if (!opts) {
		return NULL;
	}

	faup_options_defaults(opts);

	return opts;
}

void faup_options_free(faup_options_t *opts)
{
	int i = 0;
	if (opts) {
		faup_options_disable_tld_above_one(opts);
	}

	// Clear modules arguments
	for (i = 0; i < opts->modules_argc; i++) {
		free(opts->modules_argv[i]);
	}

	free(opts->snapshot_name);

	free(opts);
}
void faup_options_defaults(faup_options_t *opts)
{
 	opts->print_header = 0;
	opts->print_line = 0;
	opts->sep_char = ',';
	opts->input_source = FAUP_INPUT_SOURCE_ARGUMENT;
	opts->current_line = 1;
	opts->output = FAUP_OUTPUT_NONE;
	opts->exec_modules = FAUP_MODULES_NOEXEC;
	opts->modules_argv = NULL;
	opts->modules_argc = 0;
	opts->number_of_chars_to_remove = 0;

	faup_options_enable_tld_above_one(opts);

	opts->fields = FAUP_URL_FIELD_SCHEME | FAUP_URL_FIELD_CREDENTIAL | FAUP_URL_FIELD_SUBDOMAIN | FAUP_URL_FIELD_DOMAIN | FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD | FAUP_URL_FIELD_HOST | FAUP_URL_FIELD_TLD | FAUP_URL_FIELD_PORT | FAUP_URL_FIELD_RESOURCE_PATH | FAUP_URL_FIELD_QUERY_STRING | FAUP_URL_FIELD_FRAGMENT | FAUP_URL_FIELD_URL_TYPE;

	opts->snapshot_name = NULL;
	opts->quiet = 0;
	opts->empty_lines_processed = 0;
}

void faup_options_enable_tld_above_one(faup_options_t *opts)
{
	opts->tld_greater_extraction = 1;
	opts->tld_tree = faup_tld_tree_new();
}

void faup_options_disable_tld_above_one(faup_options_t *opts)
{
	opts->tld_greater_extraction = 0;
	faup_tld_tree_free(opts->tld_tree, NULL, 0);
	opts->tld_tree = NULL;
}

void faup_options_debug(faup_options_t *opts)
{
	printf("opts->print_header:%d\n", opts->print_header);
	printf("opts->print_line:%d\n", opts->print_line);
	printf("opts->sep_char:%c\n", opts->sep_char);
	printf("opts->output:%d\n", opts->output);
	printf("opts->tld_greater_extraction:%d\n", opts->tld_greater_extraction);
}

int faup_options_url_field_has_greater_than(faup_options_t *opts, faup_url_field_t field)
{
	int retval = opts->fields >= (2 * field) ? 1 : 0;

	return retval;
}


faup_feature_t faup_options_field_get_feature(faup_handler_t const *fh, faup_url_field_t field)
{
	faup_feature_t error;
	error.pos = -1;
	error.size = -1;

	if (field & FAUP_URL_FIELD_SCHEME) {
		return fh->faup.features.scheme;
	}
	if (field & FAUP_URL_FIELD_CREDENTIAL) {
		return fh->faup.features.credential;
	}
	if (field & FAUP_URL_FIELD_SUBDOMAIN) {
		return fh->faup.features.subdomain;
	}
	if (field & FAUP_URL_FIELD_DOMAIN) {
		return fh->faup.features.domain;
	}
	if (field & FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD) {
		return fh->faup.features.domain_without_tld;
	}
	if (field & FAUP_URL_FIELD_HOST) {
		return fh->faup.features.host;
	}
	if (field & FAUP_URL_FIELD_TLD) {
		return fh->faup.features.tld;
	}
	if (field & FAUP_URL_FIELD_PORT) {
		return fh->faup.features.port;
	}
	if (field & FAUP_URL_FIELD_RESOURCE_PATH) {
		return fh->faup.features.resource_path;
	}
	if (field & FAUP_URL_FIELD_QUERY_STRING) {
		return fh->faup.features.query_string;
	}
	if (field & FAUP_URL_FIELD_FRAGMENT) {
		return fh->faup.features.fragment;
	}
	if (field & FAUP_URL_FIELD_URL_TYPE) {
		return error;
	}

	return error;
}

faup_url_field_t faup_options_get_field_from_name(char *name)
{
  	if (!strcmp("scheme", name)) {
  		return FAUP_URL_FIELD_SCHEME;
  	}
  	if (!strcmp("credential", name)) {
  		return FAUP_URL_FIELD_CREDENTIAL;
  	}
  	if (!strcmp("subdomain", name)) {
  		return FAUP_URL_FIELD_SUBDOMAIN;
  	}
  	if (!strcmp("domain", name)) {
  		return FAUP_URL_FIELD_DOMAIN;
  	}
  	if (!strcmp("domain_without_tld", name)) {
  		return FAUP_URL_FIELD_DOMAIN_WITHOUT_TLD;
  	}
  	if (!strcmp("host", name)) {
  		return FAUP_URL_FIELD_HOST;
  	}
  	if (!strcmp("tld", name)) {
  		return FAUP_URL_FIELD_TLD;
  	}
  	if (!strcmp("port", name)) {
  		return FAUP_URL_FIELD_PORT;
  	}
  	if (!strcmp("resource_path", name)) {
  		return FAUP_URL_FIELD_RESOURCE_PATH;
  	}
  	if (!strcmp("query_string", name)) {
  		return FAUP_URL_FIELD_QUERY_STRING;
  	}
  	if (!strcmp("fragment", name)) {
  		return FAUP_URL_FIELD_FRAGMENT;
  	}
  	if (!strcmp("url_type", name)) {
  		return FAUP_URL_FIELD_URL_TYPE;
  	}

  	return FAUP_URL_FIELD_NONE;
}

faup_output_t faup_options_get_output_from_name(char *name)
{
	if (!strcmp("csv", name)) {
		return FAUP_OUTPUT_CSV;
	}
	if (!strcmp("json", name)) {
		return FAUP_OUTPUT_JSON;
	}
	if (!strcmp("module", name)) {
		return FAUP_OUTPUT_MODULE;
	}

	return FAUP_OUTPUT_NONE;
}
