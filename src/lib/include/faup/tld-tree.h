/*
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * Version 2, December 2004 
 *
 * Copyright (C) 2012-2013 Sebastien Tricaud <sebastien@honeynet.org>
 * Copyright (C) 2013 Cedric LE ROUX <cedric.lrx@gmail.com>
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
 */
 
#ifndef _H_FAUP_TLDINIT_
#define _H_FAUP_TLDINIT_

#define GENERIC_BUFF 2048

typedef struct TLDNode {
	char c;
	int EoT;      // used as a bool. End of TLD marker : ".uk", ".co.uk" => same shared root (.uk)
	struct TLDNode *sibling;
	struct TLDNode *kid;  
} TLDNode;

TLDNode *faup_tld_tree_new(void);
int get_tld_pos(TLDNode *, char *);

#endif
