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

#include <faup/options.h>

void faup_options_defaults(faup_options_t *opts)
{
 	opts->print_header = 0;
	opts->print_line = 0;
	opts->sep_char = ',';
	opts->output = FAUP_OUTPUT_CSV;
	opts->tld_greater_extraction = 0;
}

void faup_options_debug(faup_options_t *opts)
{
	printf("opts->print_header:%d\n", opts->print_header);
	printf("opts->print_line:%d\n", opts->print_line);
	printf("opts->sep_char:%c\n", opts->sep_char);
	printf("opts->output:%d\n", opts->output);
	printf("opts->tld_greater_extraction:%d\n", opts->tld_greater_extraction);
}
