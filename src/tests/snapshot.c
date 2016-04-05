#include <stdio.h>

#include <faup/snapshot.h>
#include <faup/snapshot-file.h>

int main(int argc, char **argv)
{
  faup_snapshot_t *snapshot;
  faup_snapshot_t *snapshot_cmp;

  struct htable_iter iter;
  
  size_t counter;
  
  printf("Creating snapshort test\n");
  
  snapshot = faup_snapshot_open("test");
  faup_snapshot_append(snapshot, "foo", "bar");
  faup_snapshot_append(snapshot, "ahah", "ihih");
  faup_snapshot_append(snapshot, "ahah", "ihih");
  faup_snapshot_append(snapshot, "ahah", "hehe");

  faup_snapshot_debug(snapshot);
  faup_snapshot_write(snapshot, NULL);
  faup_snapshot_close(snapshot);

  printf("Snapshot closed\n");

  snapshot = faup_snapshot_read("test");

  faup_snapshot_debug(snapshot);
  
  faup_snapshot_close(snapshot);

  snapshot = faup_snapshot_open("arf");
  faup_snapshot_append(snapshot, "foo", "bar");
  faup_snapshot_append(snapshot, "ahah", "mouarf"); // A
  faup_snapshot_append(snapshot, "ahah", "ihih");
  faup_snapshot_append(snapshot, "ahah", "tagada"); // A
  faup_snapshot_append(snapshot, "jean", "nemard");
  faup_snapshot_append(snapshot, "jean", "fonce");
  faup_snapshot_write(snapshot, NULL);

  faup_snapshot_debug(snapshot);

  faup_snapshot_close(snapshot);

  snapshot = faup_snapshot_read("test");
  faup_snapshot_close(snapshot);
  
  //#if 0  
  snapshot_cmp = faup_snapshot_compare("test", "arf");

  printf("\nComparison function:\n");
  printf("====================\n");
  faup_snapshot_debug(snapshot_cmp);
  printf("\n===== END OF COMPARISON =====\n");

  faup_snapshot_close(snapshot_cmp);
  //#endif
  
  return 0;
}
