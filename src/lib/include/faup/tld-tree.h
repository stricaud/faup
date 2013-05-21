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

#ifndef _FAUP_TLD_TREE_H_
#define _FAUP_TLD_TREE_H_

#include <stdint.h>

#include <faup/faup.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TLDNode {
	char c;
	int EoT;      // used as a bool. End of TLD marker : ".uk", ".co.uk" => same shared root (.uk)
	struct TLDNode *sibling;
	struct TLDNode *kid;  
} TLDNode;

struct _faup_tld_tree_extracted_t {
	int32_t pos;
	uint32_t size;
};
typedef struct _faup_tld_tree_extracted_t faup_tld_tree_extracted_t;

TLDNode *faup_tld_tree_new(void);
faup_tld_tree_extracted_t faup_tld_tree_extract(faup_handler_t *fh, TLDNode *tld_tree, const char *org_str);

#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_TLD_TREE_H_ */