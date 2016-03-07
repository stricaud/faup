/*
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * Version 2, December 2004 
 *
 * Copyright (C) 2012-2016 Sebastien Tricaud <sebastien@honeynet.org> 
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

#ifndef _SNAPSHOT_FILE_FAUP_H_
#define _SNAPSHOT_FILE_FAUP_H_

#include <faup/snapshot.h>

#ifdef __cplusplus
extern "C" {
#endif

int faup_snapshot_write(faup_snapshot_t *snapshot, char *workdir);
faup_snapshot_t *faup_snapshot_read(char *dirpath);
faup_snapshot_t *faup_snapshot_compare(char *snapshot_dir_a, char *snapshot_dir_b);

#ifdef __cplusplus
}
#endif

#endif	/* _SNAPSHOT_FILE_FAUP_H_ */
