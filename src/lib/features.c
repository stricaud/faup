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
#include <ctype.h>
#include <stdlib.h>

#include <faup/faup.h>
#include <faup/features.h>

void faup_features_init(faup_features_t* features)
{
	features->scheme.pos             = -1;
	features->hierarchical.pos       = -1;
	features->credential.pos         = -1;
	features->host.pos               = -1;
	features->subdomain.pos          = -1;
	features->domain.pos             = -1;
	features->domain_without_tld.pos = -1;
	features->port.pos               = -1;
	features->resource_path.pos      = -1;
	features->query_string.pos       = -1;
	features->fragment.pos           = -1;
	features->tld.pos                = -1;

	features->scheme.field             = FAUP_FEATURES_FIELD_SCHEME;
	features->hierarchical.field       = FAUP_FEATURES_FIELD_HIERARCHICAL;
	features->credential.field         = FAUP_FEATURES_FIELD_CREDENTIAL;
	features->host.field               = FAUP_FEATURES_FIELD_HOST;
	features->subdomain.field          = FAUP_FEATURES_FIELD_SUBDOMAIN;
	features->domain.field             = FAUP_FEATURES_FIELD_DOMAIN;
	features->domain_without_tld.field = FAUP_FEATURES_FIELD_DOMAIN_WITHOUT_TLD;
	features->port.field               = FAUP_FEATURES_FIELD_PORT;
	features->resource_path.field      = FAUP_FEATURES_FIELD_RESOURCE_PATH;
	features->query_string.field       = FAUP_FEATURES_FIELD_QUERY_STRING;
	features->fragment.field           = FAUP_FEATURES_FIELD_FRAGMENT;
	features->tld.field                = FAUP_FEATURES_FIELD_TLD;
}

#ifdef WIN32
static __inline char get_last_c(const char *buffer, size_t pos)
#else
static inline char get_last_c(const char *buffer, size_t pos)
#endif
{
	if (pos > 0) {
		return buffer[pos-1];
	}

	return -1;
}

int faup_features_exist(faup_feature_t feature) 
{
	return (feature.pos >= 0);
}

int faup_features_errors_lookup(faup_features_t const* url_features) 
{
	/* if ((faup_features_exist(url_features.scheme)) && */
	/*     (!faup_features_exist(url_features.hierarchical))) { */
	/* 	fprintf(stderr, "url error: can't have a scheme without a hierarchical!\n"); */
	/* 	return 1; */
	/* } */

	return 0;
}

void faup_features_find(faup_handler_t *fh, const char *url, const size_t url_len)
{
	faup_features_t* url_features = &fh->faup.features;
	char c;
	char next_c;
	size_t nb_slashes = 0;
	//int char_counter[128];
	int last_slash_pos = 0;
	const char *url_o = url;	/* We keep the original pointer as we move it */
	ssize_t whatever_len = 0;

	faup_last_slash_t last_slash_meaning = FAUP_LAST_SLASH_NOTFOUND;
	char host_is_ipv6 = 0;

	ssize_t current_pos = 0;
	ssize_t buffer_pos = 0;
	size_t i;

	size_t special_char_after_colons_pos = -1;


	faup_features_init(url_features);

	for (i = 0; i < url_len; i++) {
		c = url[i];

		if ((i + 1) < url_len) {
			next_c = url[i+1];
		} else {
			next_c = '\0';
		}

		if (c == '/') {
			nb_slashes++;
		}

		/* printf("reading %c, bufferpos=%d\n", c, buffer_pos); */

		if (host_is_ipv6 && c != ']') {
			current_pos++;
		        continue;
		}

		switch(c) {
			case '/':
				// This is for URLs such as "http://test:\/test@example.com"
				if (get_last_c(url_o, current_pos) == '\\') {
					buffer_pos=-1;
					break;
				}
				/* If it is the first time we have a '/' and previous char is ':' */
				if ((nb_slashes == 1) && (get_last_c(url_o, current_pos) == ':')) {
					if (last_slash_meaning < FAUP_LAST_SLASH_AFTER_DOMAIN) {
						last_slash_meaning = FAUP_LAST_SLASH_HIERARCHICAL;
						url_features->hierarchical.pos = current_pos -1;
						c = get_last_c(url_o, current_pos - 1);
						if (isalnum(c)) {
							url_features->scheme.pos = 0;
						}
						url_features->host.pos = -1; /* So finally we don't start with a host */
						url_features->port.pos = -1; /* So the last ':' we've found was not for a port but for  */
					} /* if (last_slash_meaning < FAUP_LAST_SLASH_AFTER_DOMAIN) */
				} else {
					/* We now check for the resource path */
					if (!faup_features_exist(url_features->resource_path)) {
						if (!faup_features_exist(url_features->scheme)) {
							if (!faup_features_exist(url_features->hierarchical)) {
								/* This host has a '/' with no hierarchy */
								/* The seen '/' is not a hierarchy so it is something like foo/bar.html */
								last_slash_meaning = FAUP_LAST_SLASH_AFTER_DOMAIN;
								url_features->resource_path.pos = current_pos;
							}
						} else {
							if (faup_features_exist(url_features->host)) {
								last_slash_meaning = FAUP_LAST_SLASH_AFTER_DOMAIN;
								url_features->resource_path.pos = current_pos;
							}
						}
					}
				}

				last_slash_pos = current_pos;

				if (faup_features_exist(url_features->host)) {
					last_slash_meaning = FAUP_LAST_SLASH_AFTER_DOMAIN;
				}

				buffer_pos=-1;
				break;
			case '@':
				if (last_slash_meaning < FAUP_LAST_SLASH_AFTER_DOMAIN) {
					if (special_char_after_colons_pos != current_pos) {
						if (!faup_features_exist(url_features->credential)) {
							if (get_last_c(url_o, current_pos) == '@') {
								break;
							}

							whatever_len = buffer_pos;
							if ((last_slash_meaning == FAUP_LAST_SLASH_HIERARCHICAL) || /* This '@' belongs to the authentication if http://foo:bar@host/blah */
									(last_slash_meaning == FAUP_LAST_SLASH_NOTFOUND)) {     /* This '@' belongs to the authentication if foo:bar@host/blah */
								url_features->credential.pos = url_features->host.pos; /* The credential starts where we thought it was a pos */
								url_features->host.pos = current_pos + 1;
								url_features->port.pos = -1; /* So the last ':' we've found was not for a port but for credential */
							} else {
								if (special_char_after_colons_pos != url_features->hierarchical.pos) {
									// That '/' belongs to the password after colons ':'
									last_slash_meaning = FAUP_LAST_SLASH_HIERARCHICAL;
									url_features->credential.pos = url_features->host.pos; 
									url_features->host.pos = current_pos + 1;
									url_features->port.pos = -1;
									url_features->resource_path.pos = -1;
								}
							}
						}
					}
				}
				buffer_pos=-1;
				break;
			case ':':
				/* We have three cases here:
				   - a ':' for the credential
				   - a ':' for the port number
				   - a ':' in the query request */
				if (!faup_features_exist(url_features->port)) {
					if (last_slash_meaning < FAUP_LAST_SLASH_AFTER_DOMAIN) {
						if (isalnum(next_c)) {
							// Skip a special char that may come after a port. Thus, this would not be a port.
							url_features->port.pos = current_pos + 1;
						} else {
						  if (next_c != ':') {
							special_char_after_colons_pos = current_pos + 1;
						  } else {
						    /* In this case we discovered a dot after the other. It it most likely an IPv6 address */
						    if (url_features->host.pos < 0) {
						      url_features->host.pos = current_pos;
						      host_is_ipv6 = 1;
						      fh->faup.url_type = FAUP_URL_IPV6;
						    }

						  }
						}
					}
				}

				buffer_pos=-1;

				break;
			case '?':
				/* printf("Current pos:%zd, special_char:%zd\n", current_pos, special_char_after_colons_pos); */
				if (special_char_after_colons_pos != current_pos) {
					if (last_slash_meaning == FAUP_LAST_SLASH_AFTER_DOMAIN) {
					  if (!faup_features_exist(url_features->query_string)) {
					    url_features->query_string.pos = current_pos;
					  }
					} else if (last_slash_meaning < FAUP_LAST_SLASH_AFTER_DOMAIN) {
					  /* printf("Before last slash after domain"); */
					  if (!faup_features_exist(url_features->resource_path)) {
						if (!faup_features_exist(url_features->scheme)) {
							if (!faup_features_exist(url_features->hierarchical)) {
								/* This host has a '/' with no hierarchy */
								/* The seen '/' is not a hierarchy so it is something like foo/bar.html */
								last_slash_meaning = FAUP_LAST_SLASH_AFTER_DOMAIN;
								if (!faup_features_exist(url_features->query_string)) {
								  url_features->query_string.pos = current_pos;
								}
							}
						} else {
							if (faup_features_exist(url_features->host)) {
								last_slash_meaning = FAUP_LAST_SLASH_AFTER_DOMAIN;
								if (!faup_features_exist(url_features->query_string)) {
								  url_features->query_string.pos = current_pos;
								}
							}
						}
					}

					}
					
				}

				buffer_pos=-1;
				break;
			case '#':
				if (special_char_after_colons_pos != current_pos) {
				  /* I was checking for FAUP_LAST_SLASH_AFTER_DOMAIN for a reason I ignore. I removed that check and all the tests are passing. \o/ */
				  /* if ((last_slash_meaning == FAUP_LAST_SLASH_AFTER_DOMAIN) && (!faup_features_exist(url_features->fragment))) { */
				  if (!faup_features_exist(url_features->fragment)) {
				    url_features->fragment.pos = current_pos;
				  }
				}

				buffer_pos=-1;
				break;
		    case '[': /* This can be an IPv6 URL, see RFC 2732*/
				if (url_features->host.pos < 0) {
				  url_features->host.pos = current_pos;
				  host_is_ipv6 = 1;
				}
			    break;
		    case ']':
			    host_is_ipv6 = 0; /* We stop handle the special IPv6 case*/
			    break;
			default:
				//fh->allocated_buf[buffer_pos] = c;
				if (current_pos == 0) {
					/* We assume we have a host to start. We shall turn it back to -1 if we have a scheme or hierachy */
					url_features->host.pos = 0;
				}
				/* We have a scheme, but no host nor no credential, then host is current_pos until we have a credential to remove it */
				if ((!faup_features_exist(url_features->host)) && 
						(!faup_features_exist(url_features->credential))) {
					url_features->host.pos = current_pos;

				}

				break;
		}

		buffer_pos++;
		current_pos++;
	}

		// faup_features_debug("features", &fh->faup.features);
}

void faup_features_debug_print(char *string, int32_t pos, uint32_t size)
{
  if (pos >= 0) {
    fprintf(stdout, "%s:%d,%u\n", string, pos, size);               
  }
}

char *faup_features_get_field_name(faup_feature_t feature) 
{
	switch(feature.field) {
		case FAUP_FEATURES_FIELD_SCHEME:
			return "scheme";
			break;
		case FAUP_FEATURES_FIELD_HIERARCHICAL:
			return "hierarchical";
			break;
		case FAUP_FEATURES_FIELD_CREDENTIAL:
			return "credential";
			break;
		case FAUP_FEATURES_FIELD_SUBDOMAIN:
			return "subdomain";
			break;
		case FAUP_FEATURES_FIELD_DOMAIN:
			return "domain";
			break;
		case FAUP_FEATURES_FIELD_DOMAIN_WITHOUT_TLD:
			return "domain_without_tld";
			break;
		case FAUP_FEATURES_FIELD_HOST:
			return "host";
			break;
		case FAUP_FEATURES_FIELD_TLD:
			return "tld";
			break;
		case FAUP_FEATURES_FIELD_PORT:
			return "port";
			break;
		case FAUP_FEATURES_FIELD_RESOURCE_PATH:
			return "resource_path";
			break;
		case FAUP_FEATURES_FIELD_QUERY_STRING:
			return "query_string";
			break;
		case FAUP_FEATURES_FIELD_FRAGMENT:
			return "fragment";
			break;		
		default:
			return "Unknown field!";
	}
}


void faup_features_debug(const char *url, faup_features_t const* features)
{
	fprintf(stdout, "url:%s\n", url);
	faup_features_debug_print("features->scheme", features->scheme.pos, features->scheme.size);
	faup_features_debug_print("features->hierarchical", features->hierarchical.pos, features->hierarchical.size);
	faup_features_debug_print("features->credential", features->credential.pos, features->credential.size);
	faup_features_debug_print("features->host", features->host.pos, features->host.size);
	faup_features_debug_print("features->domain", features->domain.pos, features->domain.size);
	faup_features_debug_print("features->domain_without_tld", features->domain_without_tld.pos, features->domain_without_tld.size);
	faup_features_debug_print("features->subdomain", features->subdomain.pos, features->subdomain.size);
	faup_features_debug_print("features->tld", features->tld.pos, features->tld.size);
	faup_features_debug_print("features->port", features->port.pos, features->port.size);
	faup_features_debug_print("features->resource_path", features->resource_path.pos, features->resource_path.size);
	faup_features_debug_print("features->query_string", features->query_string.pos, features->query_string.size);
	faup_features_debug_print("features->fragment", features->fragment.pos, features->fragment.size);
}

char *_get_feature_string(faup_handler_t *fh, faup_feature_t feature)
{
  char *retstring = NULL;
  if (feature.pos < 0) return NULL;
  if (feature.size <= 0) return NULL;
  
  retstring = malloc(feature.size + 1);
  retstring[feature.size] = '\0';
  return memcpy(retstring, fh->faup.org_str + feature.pos, feature.size);
}

char *faup_features_get_string(faup_handler_t *fh, faup_features_field_t field)
{
  char *retstring = NULL;

  switch(field) {
  case FAUP_FEATURES_FIELD_SCHEME:
    return _get_feature_string(fh, fh->faup.features.scheme);
    break;
  case FAUP_FEATURES_FIELD_HIERARCHICAL:
    return _get_feature_string(fh, fh->faup.features.hierarchical);
    break;
  case FAUP_FEATURES_FIELD_CREDENTIAL:
    return _get_feature_string(fh, fh->faup.features.credential);
    break;
  case FAUP_FEATURES_FIELD_SUBDOMAIN:
    return _get_feature_string(fh, fh->faup.features.subdomain);
    break;
  case FAUP_FEATURES_FIELD_DOMAIN:
    return _get_feature_string(fh, fh->faup.features.domain);
    break;
  case FAUP_FEATURES_FIELD_DOMAIN_WITHOUT_TLD:
    return _get_feature_string(fh, fh->faup.features.domain_without_tld);
    break;
  case FAUP_FEATURES_FIELD_HOST:
    return _get_feature_string(fh, fh->faup.features.host);
    break;
  case FAUP_FEATURES_FIELD_TLD:
    return _get_feature_string(fh, fh->faup.features.tld);
    break;
  case FAUP_FEATURES_FIELD_PORT:
    return _get_feature_string(fh, fh->faup.features.port);
    break;
  case FAUP_FEATURES_FIELD_RESOURCE_PATH:
    return _get_feature_string(fh, fh->faup.features.resource_path);
    break;
  case FAUP_FEATURES_FIELD_QUERY_STRING:
    return _get_feature_string(fh, fh->faup.features.query_string);
    break;
  case FAUP_FEATURES_FIELD_FRAGMENT:
    return _get_feature_string(fh, fh->faup.features.fragment);
    break;
  }

  return NULL;
}
