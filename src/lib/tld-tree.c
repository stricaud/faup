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

int _allocateKid(TLDNode **Node, char c, bool EoT, bool move_cursor)
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

int _allocateSibling(TLDNode **Node, char c, bool EoT, bool move_cursor)
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
int _addNode(TLDNode **Tree, char *TLD, int tLen)
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
			ret = _allocateKid(&pNode, *p, lastChar | nextIsDot | nextIsException, true);
			if( ret == -1 )
				return -1;
		}
		// char may exist in the Trie
		else 
		{
			pNode = pNode->kid;

			while( (pNode->sibling != NULL) && (pNode->c != *p) )
				pNode = pNode->sibling;

			// char does not exist
			if( pNode->c != *p )
			{
				ret = _allocateSibling(&pNode, *p, lastChar | nextIsDot | nextIsException, true);
				if( ret == -1 )
					return -1;
			}
			// char already exist at that position but not necessary as an ended one.
			else if( lastChar || nextIsDot || nextIsException )
				pNode->EoT = true;
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
int faup_tld_tree_add_node(TLDNode **Tree, char *TLD, int tLen)
{
	TLDNode *pNode;

	// regular tld
	if( *TLD != '!' ) 
	{
		// First node
		if( (*Tree)->kid == NULL ) 
		{
			(*Tree)->kid = calloc(1, sizeof(TLDNode));
			if( (*Tree)->kid == NULL )
				return -1;

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
			if( (*Tree)->sibling == NULL )
				return -1;

			(*Tree)->sibling->c = '\0'; 
		}
		pNode = (*Tree)->sibling;
	}

	return _addNode(&pNode, TLD, tLen);
}



void faup_tld_tree_add_tld(char *tld, void *user_data)
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


/*
 * Return TRUE if the provided tld is found in the provided Tree.
 * or if the provided tld match a wildcard tld in the Tree.
 * FALSE in any other case (no match)
 *
 */
bool inTrie(TLDNode *Tree, char *tld)
{
	TLDNode *pNode = Tree;
	char *p;
	int tLen;
	bool wildcard;

	tLen = strlen(tld);
	p    = tld + tLen-1;
	while( tLen-- )
	{
		wildcard = false;
		pNode    = pNode->kid;

		while( pNode && (pNode->c != *p) )
		{
			if( pNode->c == '*' )
				wildcard = true;
			pNode = pNode->sibling;
		}
		if( ! pNode )
		{
			if( wildcard ) 
			{
				while( tLen-- )
				{
					if( tLen && (*(--p) == '.') )
						return false;
				}
				return true;
			}
			return false;
		}
		p--;
	}

	if( pNode->EoT )
		return true;
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
faup_tld_tree_extracted_t faup_tld_tree_extract(TLDNode *tld_tree, const char *org_str, faup_feature_t domain)
{
	const char *p;
	char *last;
	bool found;
	int step;
	faup_tld_tree_extracted_t tld_extracted;
 
	tld_extracted.pos = -1;
	tld_extracted.size = 0;

	last = NULL;
	p    = org_str + strlen(org_str) -1;
	while( *p )
	{
		while( *(p-1) && (*p != '.') )
			p--;

		step = ( *p == '.' ) ? 1 : 0;

		found = inTrie(tld_tree->kid, (char *)p+step);
		if( ! found )
			break;
		last = p+step;
		p--;
	}

	if( last == NULL )
		return tld_extracted;

	// here we have the longest TLD
	// but is that an exception ? (ex: !siemens.om vs *.om)
	found = inTrie(tld_tree->sibling, last);

	if( found )
	{
		while( *(last) != '.' )
			last++;
		last++;
	}
	//return (strlen(org_str) - strlen(last));
	return tld_extracted;
}

