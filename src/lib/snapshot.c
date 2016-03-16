#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#define __USE_XOPEN_EXTENDED // for strdup() BS glibc now needs
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <faup/snapshot.h>


faup_snapshot_t *faup_snapshot_new(void)
{
  faup_snapshot_t *snapshot;
  snapshot = malloc(sizeof(*snapshot));
  snapshot->length = 0;
  snapshot->items = NULL;

  return snapshot;
}

void faup_snapshot_value_count_debug(faup_snapshot_value_count_t *vc)
{
  if (!vc) {
    fprintf(stderr, "%s: No such value_count object\n", __FUNCTION__);
  }
  printf("**\t\tvalue:->%s<-\n", vc->value);
  printf("**\t\tfirst time seen: %s", ctime(&vc->first_time_seen));
  printf("**\t\tlast time seen: %s", ctime(&vc->last_time_seen));
  printf("**\t\tcount:%ld\n", vc->count);
}

void faup_snapshot_item_debug(faup_snapshot_item_t *item)
{
  size_t counter;
  printf("** \titem\n");
  printf("** \tkey:%s\n", item->key);
  printf("** \tlength:%ld\n", item->length);
  for (counter = 0; counter < item->length; counter++) {
    faup_snapshot_value_count_debug(item->value_count[counter]);
  }

}

void faup_snapshot_debug(faup_snapshot_t *snapshot)
{
  size_t counter;

  printf("** Snapshot debug:");
  printf("** items length:%ld\n", snapshot->length);
  for (counter = 0; counter < snapshot->length; counter++) {
    faup_snapshot_item_debug(snapshot->items[counter]);
  }
}

faup_snapshot_value_count_t *faup_snapshot_value_count_new(void)
{
  faup_snapshot_value_count_t *vc;

  vc = malloc(sizeof(faup_snapshot_value_count_t));

  vc->value = NULL;
  vc->first_time_seen = 0;
  vc->last_time_seen = 0;
  vc->count = 0;

  return vc;
}

faup_snapshot_value_count_t *faup_snapshot_value_count_copy(faup_snapshot_value_count_t *vc)
{
  faup_snapshot_value_count_t *copy;

  copy = faup_snapshot_value_count_new();
  if (!copy) {
    fprintf(stderr, "%s: could not copy object!\n", __FUNCTION__);
    return NULL;
  }
  copy->value = strdup(vc->value);
  copy->first_time_seen = vc->first_time_seen;
  copy->last_time_seen = vc->last_time_seen;
  copy->count = vc->count;

  return copy;
}

void faup_snapshot_value_count_free(faup_snapshot_value_count_t *vc)
{
  free(vc->value);
  free(vc);
}

faup_snapshot_item_t *faup_snapshot_item_new(void)
{
  faup_snapshot_item_t *item;

  item = malloc(sizeof(faup_snapshot_item_t));
  if (!item) {
    fprintf(stderr, "Cannot allocatate a snapshot_item item!\n");
    return NULL;
  }

  item->length = 0;
  item->value_count = NULL;

  return item;

}

faup_snapshot_item_t *faup_snapshot_item_copy(faup_snapshot_item_t *item)
{
  faup_snapshot_item_t *copy;
  size_t counter;

  copy = faup_snapshot_item_new();
  copy->length = item->length;
  copy->value_count = malloc(sizeof(faup_snapshot_value_count_t *) * (item->length+1));
  
  /* printf("item len:%ld\n", item->length); */
  for (counter = 0; counter < item->length; counter++) {
    faup_snapshot_value_count_t *vc = item->value_count[counter];
    copy->value_count[counter] = faup_snapshot_value_count_copy(vc);
  }

  return copy;
}

faup_snapshot_value_count_t *faup_snapshot_value_count_get(faup_snapshot_item_t *item, char *value)
{
  size_t counter;

  if (!item) {
    fprintf(stderr, "Item is empty!\n");
    return NULL;
  }

  for (counter = 0; counter < item->length; counter++) {
    if (!strcmp(item->value_count[counter]->value, value)) {
      return item->value_count[counter];
    }
  }
  return NULL;
}

int faup_snapshot_value_count_set_value(faup_snapshot_value_count_t *vc, char *value)
{
  vc->value = strdup(value);
}

int faup_snapshot_value_count_set_first_time_seen(faup_snapshot_value_count_t *vc, time_t first_time_seen)
{
  vc->first_time_seen = first_time_seen;
}

int faup_snapshot_value_count_set_last_time_seen(faup_snapshot_value_count_t *vc, time_t last_time_seen)
{
  vc->last_time_seen = last_time_seen;
}

int faup_snapshot_value_count_set_count(faup_snapshot_value_count_t *vc, size_t count)
{
  vc->count = count;
}

int faup_snapshot_value_count_append(faup_snapshot_item_t *item, char *value)
{
  faup_snapshot_value_count_t *vc;

  vc = faup_snapshot_value_count_get(item, value);
  if (!vc) {
    item->value_count = realloc(item->value_count, sizeof(faup_snapshot_value_count_t *) * (item->length+1));
    if (!item->value_count) {
      fprintf(stderr, "Cannot allocatate a value_count!\n");
      return -1;
    }

    item->value_count[item->length] = faup_snapshot_value_count_new();

    item->value_count[item->length]->value = strdup(value);
    item->value_count[item->length]->first_time_seen = item->value_count[item->length]->last_time_seen = time(NULL);
    item->value_count[item->length]->count++;

    item->length++;
  } else {
    vc->count++;
    vc->last_time_seen = time(NULL);
  }


  return 0;
}

int faup_snapshot_value_count_append_object(faup_snapshot_item_t *item, faup_snapshot_value_count_t *vc)
{
  item->value_count = realloc(item->value_count, sizeof(faup_snapshot_value_count_t *) * (item->length+1));
  if (!item->value_count) {
    fprintf(stderr, "Cannot allocatate a value_count!\n");
    return -1;
  }

  item->value_count[item->length] = vc;
  item->length++;
}

void faup_snapshot_item_free(faup_snapshot_item_t *item)
{
  size_t count;
  for (count=0; count < item->length; count++) {
    faup_snapshot_value_count_free(item->value_count[count]);
  }
  free(item->value_count);
  free(item);
}

faup_snapshot_item_t *faup_snapshot_item_get(faup_snapshot_t *snapshot, char *item_name)
{
  size_t counter;

  if (!snapshot) {
    fprintf(stderr, "Snapshot is NULL!\n");
    return NULL;
  }

  for (counter = 0; counter < snapshot->length; counter++) {
    if (!strcmp(snapshot->items[counter]->key, item_name)) {
      return snapshot->items[counter];
    }
  }

  /* fprintf(stderr, "No such item found:%s\n", item_name); */
  return NULL;
}

int faup_snapshot_item_append(faup_snapshot_t *snapshot, char *item_name)
{
  faup_snapshot_item_t *item;

  item = faup_snapshot_item_get(snapshot, item_name);
  if (!item) {
    snapshot->items = realloc(snapshot->items, sizeof(faup_snapshot_item_t *) * (snapshot->length + 1));
    if (!snapshot->items) {
      fprintf(stderr, "Cannot allocatate a snapshot_item!\n");
      return -1;
    }
    snapshot->items[snapshot->length] = faup_snapshot_item_new();
    snapshot->items[snapshot->length]->key = strdup(item_name);
    snapshot->length++;
  }
  
  return 0;
}

void faup_snapshot_free(faup_snapshot_t *snapshot)
{
  size_t counter;
  free(snapshot->name);
  for (counter = 0; counter < snapshot->length; counter++) {
    faup_snapshot_item_free(snapshot->items[counter]);
  }
  free(snapshot->items);
  for (counter = 0; counter < snapshot->length; counter++) {
    free(snapshot->items[counter]->key);
  }
  
  free(snapshot);
}

faup_snapshot_t  *faup_snapshot_open(char *name)
{
  faup_snapshot_t *snapshot;
  int retval;

  snapshot = faup_snapshot_new();
  snapshot->name = strdup(name);

  return snapshot;
}


void faup_snapshot_close(faup_snapshot_t *snapshot)
{
  faup_snapshot_free(snapshot);
}

int faup_snapshot_append(faup_snapshot_t *snapshot, char *key, char *value)
{
  faup_snapshot_item_t *item;

  faup_snapshot_item_append(snapshot, key);
  item = faup_snapshot_item_get(snapshot, key);  
  faup_snapshot_value_count_append(item, value);

  return 0;
}

int faup_snapshot_append_item(faup_snapshot_t *snapshot, char *item_name, faup_snapshot_item_t *item)
{
  if (!snapshot) {
    fprintf(stderr, "Cannot append item to an unexisting snapshot!\n");
    return -1;
  }
  
  snapshot->items = realloc(snapshot->items, sizeof(faup_snapshot_item_t *) * (snapshot->length + 1));
  if (!snapshot->items) {
    fprintf(stderr, "Cannot allocatate a snapshot_item!\n");
    return -1;
  }

  snapshot->items[snapshot->length] = item;
  snapshot->items[snapshot->length]->key = strdup(item_name);
  snapshot->length++;
  
  return 0;
  
}

