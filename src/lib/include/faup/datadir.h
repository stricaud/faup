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

#ifndef _FAUP_DATADIR_H_
#define _FAUP_DATADIR_H_

#include <faup/faup.h>

#ifdef __cplusplus
extern "C" {
#endif


char *faup_datadir_home_file_or_dir_exists(char *append);
char *faup_datadir_get_file_from_home(char *append, bool to_symlink);
char *faup_datadir_get_file(char *append, bool to_symlink);
char *faup_datadir_get_global_file(char *append);
char *faup_datadir_file_to_write(char *file, bool to_symlink);


#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_DATADIR_H_ */
