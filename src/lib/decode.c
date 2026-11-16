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

#ifdef FAUP_LUA_MODULES
#include <faup/modules.h>
#endif

#ifdef WIN32
#include <faup/compat.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

uint32_t powui(uint32_t base, uint32_t n)
{
	uint32_t ret = 1;
	uint32_t i;

	for (i = 0; i < n; i++) {
		ret *= base;
	}
	return ret;
}

bool is_ipv4(const char* str, const size_t n)
{
	// TODO: vectorize this
	uint32_t ndots = 0;
	uint32_t nip = 0;
	int32_t cur_d = 2;
	size_t i = 0;
	char prev_c = 0;

	if (n > 15) {
		return false;
	}

	while ((i < n) && isspace(str[i])) {
		i++;
	}

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

/*
 * Numeric IPv4 hosts as browsers parse them (WHATWG URL "IPv4 parser", issue #104):
 * 1 to 4 dot-separated numbers, each decimal, hexadecimal ("0x7f") or octal
 * ("0177"); the last number fills the remaining bytes. So 2130706433, 0x7f000001,
 * 0177.0.0.1, 0x7f.1 and 127.1 are all 127.0.0.1. A trailing dot is ignored.
 */
static bool is_ipv4_numeric(const char *str, const size_t n)
{
	size_t len = n;
	size_t i = 0;
	int parts = 0;
	uint64_t value[4];

	if (len == 0) {
		return false;
	}
	if (str[len - 1] == '.') {
		len--;
		if (len == 0) {
			return false;
		}
	}

	while (i <= len) {
		size_t j = i;
		int base = 10;
		uint64_t v = 0;

		if (parts == 4) {
			return false;
		}
		while ((j < len) && (str[j] != '.')) {
			j++;
		}
		if (j == i) {
			return false; /* empty number */
		}
		if ((j - i >= 2) && (str[i] == '0') && ((str[i+1] == 'x') || (str[i+1] == 'X'))) {
			base = 16;
			i += 2;
		} else if ((j - i >= 2) && (str[i] == '0')) {
			base = 8;
			i += 1;
		}
		for (; i < j; i++) {
			int d;
			char c = str[i];
			if ((c >= '0') && (c <= '9')) {
				d = c - '0';
			} else if ((base == 16) && (c >= 'a') && (c <= 'f')) {
				d = c - 'a' + 10;
			} else if ((base == 16) && (c >= 'A') && (c <= 'F')) {
				d = c - 'A' + 10;
			} else {
				return false;
			}
			if (d >= base) {
				return false;
			}
			v = v * (uint64_t)base + (uint64_t)d;
			if (v > 0xFFFFFFFFULL) {
				return false;
			}
		}
		value[parts++] = v;
		i = j + 1;
	}

	/* every number but the last is one byte; the last one fills what is left */
	for (i = 0; i + 1 < (size_t)parts; i++) {
		if (value[i] > 255) {
			return false;
		}
	}
	return value[parts - 1] < (1ULL << (8 * (5 - parts)));
}

/* Validate an IPv6 literal, with or without brackets: up to 8 groups of 1-4 hex digits,
 * a single "::" allowed, an optional embedded IPv4 as last group (issue #105). */
bool is_ipv6(const char* str, const size_t n)
{
	const char *s = str;
	size_t len = n;
	size_t i = 0;
	int groups = 0;
	bool double_colon = false;

	if ((len >= 2) && (s[0] == '[')) {
		if (s[len-1] != ']') {
			return false;
		}
		s++;
		len -= 2;
	}
	if (len < 2) {
		return false;
	}

	while (i < len) {
		if (s[i] == ':') {
			if ((i + 1 < len) && (s[i+1] == ':')) {
				if (double_colon) {
					return false; /* only one "::" */
				}
				double_colon = true;
				i += 2;
				continue;
			}
			if (i == 0) {
				return false; /* leading single ':' */
			}
			i++;
			if (i == len) {
				return false; /* trailing single ':' */
			}
			continue;
		}
		{
			size_t j = i;
			size_t glen;
			while ((j < len) && (s[j] != ':')) {
				j++;
			}
			glen = j - i;
			if (memchr(s + i, '.', glen)) {
				/* embedded IPv4 ("::ffff:1.2.3.4") must be the last group */
				if ((j != len) || !is_ipv4(s + i, glen)) {
					return false;
				}
				groups += 2;
			} else {
				size_t k;
				if (glen > 4) {
					return false;
				}
				for (k = i; k < j; k++) {
					if (!isxdigit((unsigned char)s[k])) {
						return false;
					}
				}
				groups++;
			}
			if (groups > 8) {
				return false;
			}
			i = j;
		}
	}

	return double_colon ? (groups < 8) : (groups == 8);
}

const char *faup_decode(faup_handler_t *fh, const char *url, size_t url_len)
{
	uint32_t total_size = 0;
	int next_valid_token_pos = 0;
	const char *retval_url = url;

#ifdef FAUP_LUA_MODULES
	faup_modules_transformed_url_t *url_transformed_by_modules = NULL;
#endif

	faup_features_t *url_features = NULL;

	if (!url) {
		return NULL;
	}

	if (fh->options->number_of_chars_to_remove >= url_len) {
	  if (url_len > 0) {
	    fprintf(stderr, "Warning: Cannot remove more characters than the url string! Not removing any character on this url: %s\n", url);
	  }
	} else {
	  url_len -= fh->options->number_of_chars_to_remove;
	}

	fh->faup.decoded = true;
	fh->faup.url_type = FAUP_URL_HAS_NO_TLD;

#ifdef FAUP_LUA_MODULES
	if (fh->options->exec_modules != FAUP_MODULES_NOEXEC) {
		url_transformed_by_modules = faup_modules_decode_url_start(fh, url, url_len);
		if (url_transformed_by_modules) {
			fh->faup.org_str = url_transformed_by_modules->url; // FIXME: Change to 'url' when the output has changed to reflect new way of doing with lua stuff
			fh->faup.org_str_len = url_len;
			faup_features_find(fh, url_transformed_by_modules->url, url_transformed_by_modules->url_len);
			url_len = url_transformed_by_modules->url_len;
			retval_url = url_transformed_by_modules->url;
		}
	}
	if (!url_transformed_by_modules) {
		fh->faup.org_str = url;
		fh->faup.org_str_len = url_len;
		faup_features_find(fh, url, url_len);		
	}
#else
	// Nothing has been transformed, so we simply use our original url
	fh->faup.org_str = url;
	fh->faup.org_str_len = url_len;
	faup_features_find(fh, url, url_len);		
#endif // FAUP_LUA_MODULES

	url_features = &fh->faup.features;

	//FIXME: faup_features_errors_lookup _always_ return 0 => ?! This if statement is useless !
	if (!faup_features_errors_lookup(url_features)) {
		if ((faup_features_exist(url_features->scheme)) && (faup_features_exist(url_features->hierarchical))) { 
			total_size = url_features->hierarchical.pos - url_features->scheme.pos; 
			url_features->scheme.size = total_size;
		}

		/* The credential ends right before the host. Without a host after it
		 * (e.g. "git@github.com:/repo") there is no credential to extract:
		 * host.pos is -1 and the subtraction would underflow the size. */
		if ((faup_features_exist(url_features->credential)) && (faup_features_exist(url_features->host))
		    && (url_features->host.pos > url_features->credential.pos)) {
			total_size = url_features->host.pos - url_features->credential.pos - 1; 
			url_features->credential.size = total_size;
		} else {
			url_features->credential.pos = -1;
			url_features->credential.size = 0;
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
				const char *host = NULL;

				total_size = next_valid_token_pos - url_features->host.pos;
				url_features->host.size = total_size;
				/* Check if we are dealing with an IPv(4|6) */
				host = url + url_features->host.pos;

				bool ipv4_host = is_ipv4(host, total_size);
				bool ipv6_host = is_ipv6(host, total_size);
				if (!ipv4_host && !ipv6_host && is_ipv4_numeric(host, total_size)) {
					ipv4_host = true;
				}
				if (!ipv4_host && !ipv6_host) {
					uint32_t tld_pos;
					uintptr_t tld_len;
					 /* Extract the TLD now */
					const char *tld = (const char*) memrchr(host, '.', url_features->host.size);
					if (tld) {
						tld++;
						
						tld_pos = (uint32_t) (((uintptr_t)tld)-((uintptr_t)host));
						tld_len = url_features->host.size - tld_pos;

						if (tld_len>0) {
							const char* domain;
							/* We sometime have no resource_path after but a trailing slash ('www.honeynet.org/') */
							if ((tld[tld_len-1] == '/') || (tld[tld_len-1] == '?')) {
								tld_len--;
							}

							// All the features are detected, we can do some extra operations now
							if (fh->options->tld_greater_extraction) {
								faup_tld_tree_extracted_t tld_extracted = faup_tld_tree_extract(fh, fh->options->tld_tree);
								url_features->tld.pos = tld_extracted.pos;
								url_features->tld.size = tld_extracted.size;

								// Since we do not have the -t option, check if the TLD wasn't > 1
								if (tld_extracted.pos >= 0) {
									fh->faup.url_type = FAUP_URL_HAS_MOZILLA_TLD;
									tld_pos = tld_extracted.pos;
									tld_len = tld_extracted.size;
								} else {
									fh->faup.url_type = FAUP_URL_HAS_UNKNOWN_TLD;
									url_features->tld.pos = tld_pos + url_features->host.pos;
									url_features->tld.size = tld_len;
								}

							} else {
								fh->faup.url_type = FAUP_URL_HAS_UNKNOWN_TLD;
								url_features->tld.pos = tld_pos + url_features->host.pos;
								url_features->tld.size = tld_len;
							}

							// Extract the domain (google.com)
							domain = (const char*) memrchr(host, '.', url_features->host.size - tld_len - 1);
							if (domain) {
								uint32_t domain_pos = (uint32_t) (((uintptr_t)domain)-((uintptr_t)host));
								if (tld_pos > domain_pos) {
									domain_pos += url_features->host.pos + 1;
									url_features->domain.pos = domain_pos;
									// Grab the TLD with us
									url_features->domain.size = next_valid_token_pos - domain_pos;

									// subdomaing is what remains from the host
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
					if (ipv4_host) {
					  fh->faup.url_type = FAUP_URL_IPV4;
					}
					if (ipv6_host) {
					  fh->faup.url_type = FAUP_URL_IPV6;
					}
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

		// If no domain was found, and no subdomain, then our domain == host (see issue 24)
		if (!faup_features_exist(url_features->domain) && (!faup_features_exist(url_features->subdomain))) {
			url_features->domain.pos = url_features->host.pos;
			url_features->domain.size = url_features->host.size;
		}

		// URL has been analyzed so we can determine 'domain_without_tld' 
		if( faup_features_exist(url_features->domain) ) { 
			url_features->domain_without_tld.pos  = url_features->domain.pos;
			url_features->domain_without_tld.size = url_features->domain.size;

			if( faup_features_exist(url_features->tld) ) {
				url_features->domain_without_tld.size -= (url_features->tld.size +1); //+1 for the dot before the tld
			}
		}
		
		//faup_features_debug(url, url_features);
#ifdef FAUP_LUA_MODULES
		if (url_transformed_by_modules) {
		  free(url_transformed_by_modules);
		}
#endif // FAUP_LUA_MODULES
		return retval_url;
	}

	// FIXME: we never go here because of the 'return 0' just here and in error_lookup() !


	/* FIXME: Such a message should not belong to the library */
	fprintf(stderr, "Cannot parse the url: '%s'\n", url);
	return NULL;
}
