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
 *
 **********************************************************************************
 *
 * Implements the browser emulation of uri handling in some particular cases.
 * Naturally, this kind of url will be handled by faup this way:
 * $ faup http://www.example.com?foo:bar |cut -d, -f6
 * com?foo
 *
 * While we all recognized this is not a problem in faup, it is still a problem
 * since browsers such as Firefox will replace www.example.com?foo:bar with
 * www.example.com/?foo:bar
 * 
 * It can be processed by faup to retrieve the TLD properly:
 * $ faup -f tld http://www.example.com/?foo:bar
 * com
 *
 * Faup is here to remove the hassle from getting a TLD, since browsers behave that
 * way, faup can emulate their behavior.
 * For the moment, only firefox is implemented. The user tool must provide a flag
 * that can take two strings: firefox or any.
 * To emulate firefox, simply type faup -b firefox
 * To emulate any kind of browser, simply type faup -b any
 * 'any' will summarize the best of the different behaviors among the different browsers
 * 
 * A funny thing would be to be able to recognize the browser of our client is using just
 * by looking at DNS requests :)
 *
 * Resources:
 * http://mxr.mozilla.org/mozilla-central/source/netwerk/base/public/nsIURI.idl 
 * http://blogs.msdn.com/b/ie/archive/2005/08/15/452006.aspx
 *
 * Security issues to consider:
 * http://archive.cert.uni-stuttgart.de/bugtraq/2005/02/msg00123.html
 */

#include <stdlib.h>
#include <ctype.h>

#include <faup/faup.h>

#include <faup/emulation.h>

/* For internet explorer, the following URIs are all equivalent:
 *
 *  http://www.example.com/sample/
 *  http://www.EXAMPLE.com/sample/#
 *  Http://www.ex%41mple.com/s%61mple/
 *  http://www.example.com/sample/arbitrary/.%2e/
 */
char *faup_emulation_internet_explorer(faup_handler_t const* fh, const faup_feature_t feature)
{
	char *copied_string;
	char *new_string;
	char *tmpbuffer;
	uint32_t feature_readpos = 0;

	if (!faup_features_exist(feature)) {
		return NULL;
	}

	tmpbuffer = malloc(1024);

	new_string = malloc(feature.size);
	copied_string = malloc(feature.size);

	// Copy our string
	while (feature_readpos < feature.size) {
		new_string[feature_readpos] = fh->faup.org_str[feature.pos + feature_readpos];
		feature_readpos++;
	}
	new_string[feature_readpos] = '\0';
	copied_string = strdup(new_string);

	//	Handle the blah/# to blah/ case
	if (feature.field == FAUP_FEATURES_FIELD_FRAGMENT) {
		if (feature.size == 1) {
			new_string[0] = '\0';
		}
	}

	// Handle %41 -> a stuff
	feature_readpos = 0;
	while (feature_readpos < feature.size) {
		if (new_string[feature_readpos] == '%') {
			// We don't want blah.loca%4
			if (feature_readpos < feature.size - 2) {
				if (ishexnumber(new_string[feature_readpos + 1]) && ishexnumber(new_string[feature_readpos + 2])) {
					int converted;
					uint32_t new_readpos = 1;

					tmpbuffer[0] = new_string[feature_readpos + 1];
					tmpbuffer[1] = new_string[feature_readpos + 2];
					tmpbuffer[2] = '\0';

					converted = (int)strtol(tmpbuffer, (char **)NULL, 16);
					new_string[feature_readpos] = converted;

					// We move the following chars backwards 2 chars (bl%41h become blAh, not blA41h)
					while (new_string[feature_readpos + new_readpos + 2]) {
						new_string[feature_readpos+new_readpos] = new_string[feature_readpos+new_readpos+2];
						new_readpos++;
					}
					new_string[feature_readpos + new_readpos] = '\0';

					feature_readpos += 2;
				}
			}
		}
		feature_readpos++;
	}

	// Lowercase it all
	feature_readpos = 0;
	while (feature_readpos < feature.size) {
		new_string[feature_readpos] = tolower(new_string[feature_readpos]);
		feature_readpos++;
	}

	if (strcmp(new_string, copied_string)) {
		free(tmpbuffer);
		free(copied_string);
		return new_string;
	}

	free(tmpbuffer);
	free(new_string);
	free(copied_string);
	return NULL;
}

char *faup_emulation_safari(faup_handler_t const* fh, const faup_feature_t feature)
{
	return NULL;
}

char *faup_emulation_firefox(faup_handler_t const* fh, const faup_feature_t feature)
{
	return NULL;
}



