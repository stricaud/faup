#include <stdio.h>

#include <faup/snapshot.h>
#include <faup/snapshot-file.h>

int main(int argc, char **argv)
{
  faup_snapshot_t *snapshot;

  snapshot = faup_snapshot_open("test");
  faup_snapshot_append(snapshot, "foo", "bar");
  faup_snapshot_append(snapshot, "ahah", "ihih");
  faup_snapshot_append(snapshot, "ahah", "ihih");
  faup_snapshot_append(snapshot, "ahah", "hehe");

  faup_snapshot_debug(snapshot);

  faup_snapshot_write(snapshot, NULL);
  
  faup_snapshot_close(snapshot);

  snapshot = faup_snapshot_read("test");

  faup_snapshot_close(snapshot);

  return 0;
}
