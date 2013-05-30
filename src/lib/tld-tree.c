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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <faup/tld.h>
#include <faup/tld-tree.h>

#include <faup/utarray.h>

static UT_array *_tlds;

static int _faup_tld_tree_allocate_kid(TLDNode **Node, char c, bool EoT, bool move_cursor)
{
	if( (*Node)->kid != NULL )
		return -1;
	
	(*Node)->kid = calloc(1, sizeof(TLDNode));
	if( (*Node)->kid == NULL )
		return -1;

	((TLDNode *)(*Node)->kid)->c   = c;
	((TLDNode *)(*Node)->kid)->EoT = EoT;

	if( move_cursor )
		*Node = (TLDNode *)(*Node)->kid;
	return 0;
}

static int _faup_tld_tree_allocate_sibling(TLDNode **Node, char c, bool EoT, bool move_cursor)
{
	if( (*Node)->sibling != NULL )
		return -1;
	
	(*Node)->sibling = calloc(1, sizeof(TLDNode));
	if( (*Node)->sibling== NULL )
		return -1;

	((TLDNode *)(*Node)->sibling)->c   = c;
	((TLDNode *)(*Node)->sibling)->EoT = EoT;

	if( move_cursor )
		*Node = (TLDNode *)(*Node)->sibling;
	return 0;
}
			
/* 
 * Add a node the the Trie (should be kid or sibling of the root)
 *
 */
static int _faup_tld_tree_add_node(TLDNode **Tree, char *TLD, int tLen)
{
	bool lastChar, nextIsDot, nextIsException;
	char *p;
	int ret;
	TLDNode *pNode  = *Tree;


	// Add the TLD to the Trie in reverse order
	p = TLD +tLen -1;
	while( *p )
	{
		lastChar        =  *(p-1)         ? false : true;
		nextIsDot       = (*(p-1) == '.') ? true  : false;
		nextIsException = (*(p-1) == '!') ? true  : false;

		// char does not exist in the Trie at that position
		if( pNode->kid == NULL ) 
		{
			ret = _faup_tld_tree_allocate_kid(&pNode, *p, lastChar | nextIsDot | nextIsException, true);
			if( ret == -1 ) {
				return -1;
			}
		}
		// char may exist in the Trie
		else 
		{
			pNode = pNode->kid;

			while( (pNode->sibling != NULL) && (pNode->c != *p) ) {
				pNode = pNode->sibling;
			}

			// char does not exist
			if( pNode->c != *p )
			{
				ret = _faup_tld_tree_allocate_sibling(&pNode, *p, lastChar | nextIsDot | nextIsException, true);
				if( ret == -1 ) {
					return -1;
				}
			}
			// char already exist at that position but not necessary as an ended one.
			else if( lastChar || nextIsDot || nextIsException ) {
				pNode->EoT = true;
			}
		}
		p--;
	}

	return 0;
}


/*
 * Add a node to the Trie
 * Define whether it's an exception (!<domain.tld>) or a regular TLD (including wildcards ones)
 * Exception go under the Tree root's kid part, regular under the root's sibling.
 */
static int faup_tld_tree_add_node(TLDNode **Tree, char *TLD, int tLen)
{
	TLDNode *pNode;

	// regular tld
	if( *TLD != '!' ) 
	{
		// First node
		if( (*Tree)->kid == NULL ) 
		{
			(*Tree)->kid = calloc(1, sizeof(TLDNode));
			if( (*Tree)->kid == NULL ) {
				return -1;
			}

			(*Tree)->kid->c = '\0'; 
		}
		pNode = (*Tree)->kid;
	}
	// exception
	else 
	{
		if( (*Tree)->sibling == NULL ) 
		{
			(*Tree)->sibling = calloc(1, sizeof(TLDNode));
			if( (*Tree)->sibling == NULL ) {
				return -1;
			}

			(*Tree)->sibling->c = '\0'; 
		}
		pNode = (*Tree)->sibling;
	}

	return _faup_tld_tree_add_node(&pNode, TLD, tLen);
}



static void faup_tld_tree_add_tld(char *tld, void *user_data)
{
	int retval;

	TLDNode *Tree = (TLDNode *)user_data;

	retval = faup_tld_tree_add_node(&Tree, tld, strlen(tld));
	if (retval) {
		fprintf(stderr, "Error adding the tld '%s' to the tree\n", tld);
	}

}

/* Return a TLD Tree (TLDNode *) or NULL on error
 *
 * Load TLD from a char array and transform it as a 'reversed' Tree (= Trie).
 * Ex: '.com' is loaded as 'm->o->c->.'
 *
 * There is actually 2 Tree :
 * Tree->siblings contains TLD and wildcards (ex: *.om)
 * Tree->kids     contains exceptions TLD (ex: !siemens.om)
 *
 * See : 
 * - http://en.wikipedia.org/wiki/Trie
 * - http://en.wikipedia.org/wiki/Suffix_tree
 */

TLDNode *faup_tld_tree_new(void)
{
	TLDNode *Tree;
	int nbTLD, ret, i;

	// Initialize the tree
	Tree = calloc(1, sizeof(TLDNode));
	if( Tree == NULL )
		return NULL;
	Tree->c = '\0'; 

	faup_tld_array_populate();
	faup_tld_array_foreach(faup_tld_tree_add_tld, Tree);
	faup_tld_array_destroy();

	return Tree;
}


void faup_tld_tree_free(TLDNode *Tree)
{
	free(Tree);
}

/*
 * Return TRUE if the provided tld is found in the provided Tree.
 * or if the provided tld match a wildcard tld in the Tree.
 * FALSE in any other case (no match)
 *
 */
static bool faup_tld_tree_tld_exists(TLDNode *Tree, const char *tld, int tld_len)
{
	TLDNode *pNode = Tree;
	const char *p;
	bool wildcard;
	int lenght = 0;


	p = tld + tld_len - 1;
	while (tld_len--) {
		wildcard = false;
		pNode = pNode->kid;

		while( pNode && (pNode->c != *p) ) {
			if (pNode->c == '*') {
				wildcard = true;
			}
			pNode = pNode->sibling;
		}

		if( ! pNode ) {
			if( wildcard ) {
				while( tld_len-- ) {
					if( tld_len && (*(--p) == '.') ) {
						return false;
					}
				}
				return true;
			}
			return false;
		}

		p--;
	}

	if( pNode->EoT ) {
		return true;
	}

	return false;
}

/*
 * Return the starting position of the tld in host or -1 if not found
 * Require a TLD Tree.
 * Ex: 
 * - google.com => 7 (='com')
 * - abc.co.uk  => 4 (='co.uk')
 * - google.nawak => -1 (NOT FOUND)
 *
 */
faup_tld_tree_extracted_t faup_tld_tree_extract(faup_handler_t *fh, TLDNode *tld_tree, const char *org_str)
{
	const char *p;
	int32_t host_last_pos;
	char *last;
	bool found;
	uint32_t step = 0;
	uint32_t tld_len = 0;
	faup_tld_tree_extracted_t tld_extracted;

	uint32_t counter;
	bool has_a_dot = false;

	tld_extracted.pos = -1;
	tld_extracted.size = 0;

	if (!tld_tree->kid) {
		fprintf(stderr, "(Warning) Cannot extract TLD > 1. Mozilla list does not exists. Please download it (faup -u)\n");
		return tld_extracted;
	}

	last = NULL;

	host_last_pos = fh->faup.features.host.pos + fh->faup.features.host.size;

	p = org_str + host_last_pos - 1; 

	counter = fh->faup.features.host.size - 1;

	while( counter )
	{
		while( *(p-1) && (*p != '.') ) {
			p--;
			counter--;
		}

		step = ( *p == '.' ) ? 1 : 0;
		
		found = faup_tld_tree_tld_exists(tld_tree->kid, p + step, fh->faup.features.host.size - counter - 1);
		if( ! found ) {
			break;
		} else {
			tld_len = fh->faup.features.host.size - counter - 1;
		}

		last = (char *) p + step;

		p--;
		counter--;
	}

	if( last == NULL ) {
		return tld_extracted;
	}

	// here we have the longest TLD
	// but is that an exception ? (ex: !siemens.om vs *.om)
	counter = 0;
	found = faup_tld_tree_tld_exists(tld_tree->sibling, last, strlen(last));
	if( found )
	{
		while (counter < tld_len) {
			if (*last != '.') {
				last++;
			} else {
				has_a_dot = true;
			}
			counter++;
		}
	}

	if (!has_a_dot) { 
		counter = 0;
	}

	tld_extracted.size = tld_len;
	tld_extracted.pos = fh->faup.features.host.pos + fh->faup.features.host.size - tld_extracted.size + counter;

	return tld_extracted;
}

