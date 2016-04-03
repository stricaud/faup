#include <stdio.h>
#define __USE_XOPEN_EXTENDED
#include <string.h>

#include <faup/hash/hash.h>
#include <faup/hash/htable.h>

struct _foobar_t {
  char *name;
  int blah;
};
typedef struct _foobar_t foobar_t;

static size_t rehash(const void *e, void *unused)
{
  return hash_string(((foobar_t *)e)->name);
}

static bool streq(const void *e, void *string)
{
  return strcmp(((foobar_t *)e)->name, string) == 0;
}


int main(int argc, char **argv)
{
  struct htable ht;
  foobar_t *foo;
  foobar_t *ret;
  
  htable_init(&ht, rehash, NULL);
  foo = malloc(sizeof(foobar_t));
  foo->name = strdup("fumier");
  foo->blah = 123;

  htable_add(&ht, hash_string(foo->name), foo);

  ret = (foobar_t *)htable_get(&ht, hash_string("gugus"), streq, "gugus");
  if (ret) {
    printf("Found\n");
  } else {
    printf("Not found\n");
  }
  
  free(foo->name);
  free(foo);
  
  return 0;
}
