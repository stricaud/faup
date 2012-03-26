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
#include <furl/decode.h>
#include <furl/features.h>
#include <furl/handlers.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int furl_decode(furl_handler_t *fh, char *url)
{
	size_t url_len = 0;
	size_t total_size = 0;

	int next_valid_token_pos = 0;

	if (!url) {
		return FURL_URL_EMPTY;
	}

	url_len = strlen(url);

	if (url_len > FURL_MAXLEN) {
		return FURL_URL_TOOLONG;
	}

	fh->furl = malloc(sizeof(furl_t *));
	if (!fh->furl) {
		return FURL_URL_MEM_ERROR;
	} else {
		furl_features_t url_features = furl_features_find(fh, url, url_len);
		/* furl_features_debug(url, url_features); */
		if (!furl_features_errors_lookup(url_features)) {
			 if (furl_features_exist(url_features.scheme)) { 
			 	size_t total_size = url_features.hierarchical - url_features.scheme; 
			 	memcpy(fh->allocated_buf, url + url_features.scheme, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s%c", fh->allocated_buf, fh->sep_char); 
			 } else { 
			 	fprintf(stdout, "%c", fh->sep_char); 
			 } 

			 if (furl_features_exist(url_features.credential)) { 
			 	size_t total_size = url_features.domain - url_features.credential - 1; 
			 	memcpy(fh->allocated_buf, url + url_features.credential, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s%c", fh->allocated_buf, fh->sep_char); 
			 } else { 
			 	fprintf(stdout, "%c", fh->sep_char); 
			 } 

			 if (furl_features_exist(url_features.domain)) { 
			 	if (furl_features_exist(url_features.port)) { 
			 		next_valid_token_pos = url_features.port - 1; 
			 	} else if (furl_features_exist(url_features.resource_path)) { 
			 		next_valid_token_pos = url_features.resource_path; 
			 	} else if (furl_features_exist(url_features.query_string)) { 
			 		next_valid_token_pos = url_features.query_string; 
			 	} else if (furl_features_exist(url_features.fragment)) { 
			 		next_valid_token_pos = url_features.fragment; 
			 	} else { 
			 		/* /\\* We have no next token *\\/  */
			 		/* /\\* FIXME: We shall return after, no need to go further *\\/  */
			 		next_valid_token_pos = url_len; 
				}

			 	total_size = next_valid_token_pos - url_features.domain; 
			 	memcpy(fh->allocated_buf, url + url_features.domain, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s%c", fh->allocated_buf, fh->sep_char);
				{	 /* Extract the TLD now */

					char *tld = strrchr(fh->allocated_buf, '.');
					if (tld) {
						size_t tld_len = strlen(tld);
						if (tld_len>1) {
							/* We sometime have no resource_path after but a trailing slash ('www.honeynet.org/') */
							if (tld[tld_len-1] == '/') {
								tld[tld_len-1] = '\0';
							}
							*tld++;
							if (!strtod(tld, NULL)) {
								fprintf(stdout, "%s%c", tld, fh->sep_char);					
							} else {
								fprintf(stdout, "%c", fh->sep_char);
							}
						}
					} else {
						fprintf(stdout, "%c", fh->sep_char);
					}
				}

			 } else {	/* if (furl_features_exist(url_features.domain)) { */
			 	fprintf(stdout, "%c%c", fh->sep_char, fh->sep_char); 
		 	} 


			 if (furl_features_exist(url_features.port)) { 
				if (furl_features_exist(url_features.resource_path)) { 
			 		next_valid_token_pos = url_features.resource_path; 
			 	} else if (furl_features_exist(url_features.query_string)) { 
			 		next_valid_token_pos = url_features.query_string; 
			 	} else if (furl_features_exist(url_features.fragment)) { 
			 		next_valid_token_pos = url_features.fragment; 
			 	} else { 
			 		/* /\\* We have no next token *\\/  */
			 		/* /\\* FIXME: We shall return after, no need to go further *\\/  */
			 		next_valid_token_pos = url_len; 
				}
			 	total_size = next_valid_token_pos - url_features.port; 
			 	memcpy(fh->allocated_buf, url + url_features.port, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s%c", fh->allocated_buf, fh->sep_char);
			} else {
		 		fprintf(stdout, "%c", fh->sep_char); 
			}

			 if (furl_features_exist(url_features.resource_path)) { 
				if (furl_features_exist(url_features.query_string)) { 
			 		next_valid_token_pos = url_features.query_string; 
			 	} else if (furl_features_exist(url_features.fragment)) { 
			 		next_valid_token_pos = url_features.fragment; 
			 	} else { 
			 		/* /\\* We have no next token *\\/  */
			 		/* /\\* FIXME: We shall return after, no need to go further *\\/  */
			 		next_valid_token_pos = url_len; 
				}
			 	total_size = next_valid_token_pos - url_features.resource_path; 
			 	memcpy(fh->allocated_buf, url + url_features.resource_path, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s%c", fh->allocated_buf, fh->sep_char);
			} else {
		 		fprintf(stdout, "%c", fh->sep_char); 
			}

			 if (furl_features_exist(url_features.query_string)) { 
				if (furl_features_exist(url_features.fragment)) { 
			 		next_valid_token_pos = url_features.fragment; 
			 	} else { 
			 		/* /\\* We have no next token *\\/  */
			 		/* /\\* FIXME: We shall return after, no need to go further *\\/  */
			 		next_valid_token_pos = url_len; 
				}
			 	total_size = next_valid_token_pos - url_features.query_string; 
			 	memcpy(fh->allocated_buf, url + url_features.query_string, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s%c", fh->allocated_buf, fh->sep_char);
			} else {
		 		fprintf(stdout, "%c", fh->sep_char); 
			}

			 if (furl_features_exist(url_features.fragment)) { 
			 	total_size = url_len - url_features.fragment; 
			 	memcpy(fh->allocated_buf, url + url_features.fragment, total_size); 
			 	fh->allocated_buf[total_size] = '\0'; 
			 	fprintf(stdout, "%s", fh->allocated_buf, fh->sep_char);
			} else {
		 		fprintf(stdout, "%c", fh->sep_char); 
			}

			 fprintf(stdout, "\n"); 
			 return 0;
			} else {
				/* FIXME: Such a message should not belong to the library */
				fprintf(stderr, "Cannot parse the url: '%s'\n", url);
				return FURL_URL_PARSER_ERROR;
			}
	}

			

	return FURL_URL_UNKNOWN_ERROR;
}

