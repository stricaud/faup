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

#define _GNU_SOURCE
#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/features.h>
#include <faup/tld-tree.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

uint32_t powui(uint32_t base, uint32_t n)
{
	uint32_t ret = 1;
	for (uint32_t i = 0; i < n; i++) {
		ret *= base;
	}
	return ret;
}

bool is_ipv4(const char* str, const size_t n)
{
	if (n > 15) {
		return false;
	}

	// TODO: vectorize this
	uint32_t ndots = 0;
	uint32_t nip = 0;
	int32_t cur_d = 2;
	size_t i = 0;
	while ((i < n) && isspace(str[i])) {
		i++;
	}
	char prev_c = 0;
	for (; i < n; i++) {
		const char c = str[i];
		if (c == '.') {
			if (prev_c == '.') {
				return false;
			}
			ndots++;
			nip /= powui(10, cur_d+1);
			if (nip > 255) {
				return false;
			}
			nip = 0;
			cur_d = 2;
		}
		else {
			if ((c < '0') || (c > '9')) {
				if (prev_c == '.') {
					return false;
				}
				break;
			}
			if (cur_d < 0) {
				return false;
			}
			nip += ((uint32_t)((c-'0')))*powui(10, cur_d);
			cur_d--;
		}
		prev_c = c;
	}
	nip /= powui(10, cur_d+1);
	if (nip > 255) {
		return false;
	}
	// Check trailing characters
	for (; i < n; i++) {
		if (!isspace(str[i])) {
			return false;
		}
	}
	return ndots == 3;
}

int faup_decode(faup_handler_t *fh, const char *url, const size_t url_len, faup_options_t *options)
{
	uint32_t total_size = 0;

	int next_valid_token_pos = 0;

	if (!url) {
		return FAUP_URL_EMPTY;
	}

	fh->faup.org_str = url;
	faup_features_find(fh, url, url_len);
	faup_features_t* url_features = &fh->faup.features;
	if (!faup_features_errors_lookup(url_features)) {
		if ((faup_features_exist(url_features->scheme)) && (faup_features_exist(url_features->hierarchical))) { 
			total_size = url_features->hierarchical.pos - url_features->scheme.pos; 
			url_features->scheme.size = total_size;
		}

		if (faup_features_exist(url_features->credential)) { 
			total_size = url_features->host.pos - url_features->credential.pos - 1; 
			url_features->credential.size = total_size;
		}

		if (faup_features_exist(url_features->host)) { 
			if (faup_features_exist(url_features->port)) { 
				next_valid_token_pos = url_features->port.pos - 1; 
			} else if (faup_features_exist(url_features->resource_path)) { 
				next_valid_token_pos = url_features->resource_path.pos; 
			} else if (faup_features_exist(url_features->query_string)) { 
				next_valid_token_pos = url_features->query_string.pos; 
			} else if (faup_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}

			if (next_valid_token_pos > url_features->host.pos) {
				total_size = next_valid_token_pos - url_features->host.pos;
				url_features->host.size = total_size;
				/* Check if we are dealing with an IPv(4|6) */
				const char* host = url + url_features->host.pos;
				if (!is_ipv4(host, total_size)) {
					 /* Extract the TLD now */
					const char *tld = (const char*) memrchr(host, '.', url_features->host.size);
					if (tld) {
						tld++;
						uint32_t tld_pos = (uint32_t) (((uintptr_t)tld)-((uintptr_t)host));
						uintptr_t tld_len = url_features->host.size - tld_pos;
						if (tld_len>1) {
							/* We sometime have no resource_path after but a trailing slash ('www.honeynet.org/') */
							if (tld[tld_len-1] == '/') {
								tld_len--;
							}

							// All the features are detected, we can do some extra operations now
							if (options->tld_greater_extraction) {
								faup_tld_tree_extracted_t tld_extracted = faup_tld_tree_extract(options->tld_tree, fh->faup.org_str, url_features->host);
								url_features->tld.pos = tld_extracted.pos;
								url_features->tld.size = tld_extracted.size;

								// Since we have the -t option checks if the TLD wasn't > 1
								if (tld_extracted.pos >= 0) {
									tld_pos = tld_extracted.pos;
									tld_len = tld_extracted.size;
								} else {
									url_features->tld.pos = tld_pos + url_features->host.pos;
									url_features->tld.size = tld_len;
								}

							} else {
								url_features->tld.pos = tld_pos + url_features->host.pos;
								url_features->tld.size = tld_len;
							}

							// Extract the domain (google.com)
							const char* domain = (const char*) memrchr(host, '.', url_features->host.size - tld_len - 1);
							if (domain) {
								uint32_t domain_pos = (uint32_t) (((uintptr_t)domain)-((uintptr_t)host));
								if (tld_pos > domain_pos) {
									domain_pos += url_features->host.pos + 1;
									url_features->domain.pos = domain_pos;
									// Grab the TLD with us
									url_features->domain.size = next_valid_token_pos - domain_pos;

									// subdomaing is what remains of the host
									if (url_features->domain.pos > 1) {
										url_features->subdomain.pos = url_features->host.pos;
										url_features->subdomain.size = url_features->domain.pos - url_features->host.pos - 1;
									}
								}
							}
						}
					}
					else {
						// If no TLD, the domain is same as the host
						url_features->domain = url_features->host;
					}
				}
				else {
					// If this is an IPv4, put it also in the host field
					url_features->domain = url_features->host;
				}
			}
		}

		if (faup_features_exist(url_features->port)) { 
			if (faup_features_exist(url_features->resource_path)) { 
				next_valid_token_pos = url_features->resource_path.pos; 
			} else if (faup_features_exist(url_features->query_string)) { 
				next_valid_token_pos = url_features->query_string.pos; 
			} else if (faup_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}
			if (next_valid_token_pos > url_features->port.pos) {
				total_size = next_valid_token_pos - url_features->port.pos; 
				url_features->port.size = total_size;
			}
		}

		if (faup_features_exist(url_features->resource_path)) { 
			if (faup_features_exist(url_features->query_string)) { 
				next_valid_token_pos = url_features->query_string.pos; 
			} else if (faup_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}
			if (next_valid_token_pos > url_features->resource_path.pos) {
				total_size = next_valid_token_pos - url_features->resource_path.pos; 
				url_features->resource_path.size = total_size;
			}
		}

		if (faup_features_exist(url_features->query_string)) { 
			if (faup_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}
			if (next_valid_token_pos > url_features->query_string.pos) {
				total_size = next_valid_token_pos - url_features->query_string.pos; 
				url_features->query_string.size = total_size;
			}
		}

		if (faup_features_exist(url_features->fragment)) { 
			total_size = url_len - url_features->fragment.pos; 
			url_features->fragment.size = total_size;
		}

		//faup_features_debug(url, url_features);
		return 0;
	}

	/* FIXME: Such a message should not belong to the library */
	fprintf(stderr, "Cannot parse the url: '%s'\n", url);
	return FAUP_URL_PARSER_ERROR;
}
