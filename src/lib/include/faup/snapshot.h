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

#ifndef _SNAPSHOT_FAUP_H_
#define _SNAPSHOT_FAUP_H_

#include <stdio.h>
#include <time.h>

#include <faup/hash/hash.h>
#include <faup/hash/htable.h>

#include <faup/faup.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _faup_snapshot_value_count_t {
  char *value;
  time_t first_time_seen;
  time_t last_time_seen;
  size_t count;
};
typedef struct _faup_snapshot_value_count_t faup_snapshot_value_count_t;

struct _faup_snapshot_item_t {
  char *key;
  struct htable values;
  /* faup_snapshot_value_count_t **value_count; */
  size_t length;
};
typedef struct _faup_snapshot_item_t faup_snapshot_item_t;

struct _faup_snapshot_t {
  char *name;

  faup_snapshot_item_t **items;
  size_t length;
};
typedef struct _faup_snapshot_t faup_snapshot_t;


faup_snapshot_t *faup_snapshot_new(void);
void faup_snapshot_value_count_debug(faup_snapshot_value_count_t *vc);
void faup_snapshot_item_debug(faup_snapshot_item_t *item);
void faup_snapshot_debug(faup_snapshot_t *snapshot);
faup_snapshot_value_count_t *faup_snapshot_value_count_new(void);
faup_snapshot_value_count_t *faup_snapshot_value_count_copy(faup_snapshot_value_count_t *vc);
void faup_snapshot_value_count_free(faup_snapshot_value_count_t *vc);
faup_snapshot_item_t *faup_snapshot_item_new(char *key);
faup_snapshot_item_t *faup_snapshot_item_copy(faup_snapshot_item_t *item);
faup_snapshot_value_count_t *faup_snapshot_value_count_get(faup_snapshot_item_t *item, char *value);
int faup_snapshot_value_count_append(faup_snapshot_item_t *item, char *value);
int faup_snapshot_value_count_append_object(faup_snapshot_item_t *item, faup_snapshot_value_count_t *vc);
int faup_snapshot_value_count_set_value(faup_snapshot_value_count_t *vc, char *value);
int faup_snapshot_value_count_set_first_time_seen(faup_snapshot_value_count_t *vc, time_t first_time_seen);
int faup_snapshot_value_count_set_last_time_seen(faup_snapshot_value_count_t *vc, time_t last_time_seen);
int faup_snapshot_value_count_set_count(faup_snapshot_value_count_t *vc, size_t count);

void faup_snapshot_item_free(faup_snapshot_item_t *item);
faup_snapshot_item_t *faup_snapshot_item_get(faup_snapshot_t *snapshot, char *item_name);
int faup_snapshot_item_append(faup_snapshot_t *snapshot, char *item_name);
void faup_snapshot_free(faup_snapshot_t *snapshot);
faup_snapshot_t  *faup_snapshot_open(char *name);
void faup_snapshot_close(faup_snapshot_t *snapshot);
int faup_snapshot_append(faup_snapshot_t *snapshot, char *key, char *value);
int faup_snapshot_append_item(faup_snapshot_t *snapshot, char *item_name, faup_snapshot_item_t *item);
void faup_snapshot_output(faup_handler_t *fh, faup_snapshot_t *snapshot, FILE *fd);
  
#ifdef __cplusplus
}
#endif

#endif	/* _SNAPSHOT_FAUP_H_ */
