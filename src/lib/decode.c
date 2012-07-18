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

#define _GNU_SOURCE
#include <furl/furl.h>
#include <furl/decode.h>
#include <furl/features.h>
#include <furl/handlers.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int furl_decode(furl_handler_t *fh, const char *url, const size_t url_len)
{
	uint32_t total_size = 0;

	int next_valid_token_pos = 0;

	if (!url) {
		return FURL_URL_EMPTY;
	}

	fh->furl.org_str = url;
	furl_features_find(fh, url, url_len);
	furl_features_t* url_features = &fh->furl.features;
	if (!furl_features_errors_lookup(url_features)) {
		if ((furl_features_exist(url_features->scheme)) && (furl_features_exist(url_features->hierarchical))) { 
			total_size = url_features->hierarchical.pos - url_features->scheme.pos; 
			url_features->scheme.size = total_size;
		}

		if (furl_features_exist(url_features->credential)) { 
			total_size = url_features->domain.pos - url_features->credential.pos - 1; 
			url_features->credential.size = total_size;
		}

		if (furl_features_exist(url_features->domain)) { 
			if (furl_features_exist(url_features->port)) { 
				next_valid_token_pos = url_features->port.pos - 1; 
			} else if (furl_features_exist(url_features->resource_path)) { 
				next_valid_token_pos = url_features->resource_path.pos; 
			} else if (furl_features_exist(url_features->query_string)) { 
				next_valid_token_pos = url_features->query_string.pos; 
			} else if (furl_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}

			total_size = next_valid_token_pos - url_features->domain.pos;
			url_features->domain.size = total_size;
			{	 /* Extract the TLD now */
				const char* domain = url + url_features->domain.pos;
				const char *tld = (char*) memrchr(domain, '.', url_features->domain.size);
				if (tld) {
					tld++;
					uint32_t tld_pos = (uint32_t) (((uintptr_t)tld)-((uintptr_t)domain));
					uintptr_t tld_len = url_features->domain.size - tld_pos;
					if (tld_len>1) {
						/* We sometime have no resource_path after but a trailing slash ('www.honeynet.org/') */
						if (tld[tld_len-1] == '/') {
							tld_len--;
						}
						// TODO: check for numbers ?
						url_features->tld.pos = tld_pos + url_features->domain.pos;
						url_features->tld.size = tld_len;
					}
				}
			}
		}

		if (furl_features_exist(url_features->port)) { 
			if (furl_features_exist(url_features->resource_path)) { 
				next_valid_token_pos = url_features->resource_path.pos; 
			} else if (furl_features_exist(url_features->query_string)) { 
				next_valid_token_pos = url_features->query_string.pos; 
			} else if (furl_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}
			total_size = next_valid_token_pos - url_features->port.pos; 
			url_features->port.size = total_size;
		}

		if (furl_features_exist(url_features->resource_path)) { 
			if (furl_features_exist(url_features->query_string)) { 
				next_valid_token_pos = url_features->query_string.pos; 
			} else if (furl_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}
			total_size = next_valid_token_pos - url_features->resource_path.pos; 
			url_features->resource_path.size = total_size;
		}

		if (furl_features_exist(url_features->query_string)) { 
			if (furl_features_exist(url_features->fragment)) { 
				next_valid_token_pos = url_features->fragment.pos; 
			} else { 
				/* /\\* We have no next token *\\/  */
				/* /\\* FIXME: We shall return after, no need to go further *\\/  */
				next_valid_token_pos = url_len; 
			}
			total_size = next_valid_token_pos - url_features->query_string.pos; 
			url_features->query_string.size = total_size;
		}

		if (furl_features_exist(url_features->fragment)) { 
			total_size = url_len - url_features->fragment.pos; 
			url_features->fragment.size = total_size;
		}

		//furl_features_debug(url, url_features);
		return 0;
	}

	/* FIXME: Such a message should not belong to the library */
	fprintf(stderr, "Cannot parse the url: '%s'\n", url);
	return FURL_URL_PARSER_ERROR;
}
