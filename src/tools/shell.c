#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <sys/errno.h>
#include <sys/syscall.h>

#include <faup/datadir.h>
#include <faup/modules.h>
#include <faup/snapshot-file.h>

#ifdef FAUP_GTCACA
#include "browse.h"
#endif

int shell_modules_list_cb(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
  printf("[%d] %s%s%s\n", count, modules_dir, FAUP_OS_DIRSEP, module);

  return 0;
}

int faup_handle_shell_snapshot(int argc, char **argv)
{
  char *snapshot_name;
  char *action;
  faup_snapshot_t *snapshot = NULL;
  faup_snapshot_item_t *snapshot_item;
  faup_snapshot_value_count_t *vc = NULL;

  struct htable_iter iter;
  char first_timebuf[200];
  char last_timebuf[200];

  
  if (argc < 5) {
    printf("Usage: %s $ snapshot action name\n", argv[0]);
    printf("\nWhere name is your snapshot name\n");
    printf("\nWhere action can be:\n");
    printf("print: Print content of snapshot\n");
    printf("get: Grab an item key\n");
#ifdef FAUP_GTCACA
    printf("browse: Graphical snapshot browser\n");
#endif // FAUP_GTCACA
    return -1;
  }
  action = argv[3];
  snapshot_name = argv[4];

  if (!strcmp(action, "read")) { 
    snapshot = faup_snapshot_read(snapshot_name);
    if (!snapshot) {
      fprintf(stderr, "Cannot read snapshot: %s\n", snapshot_name);
      return -1;
    }
    faup_snapshot_output(NULL, snapshot, stdout);
    faup_snapshot_free(snapshot);
    return 0;
  }

  if (!strcmp(action, "get")) {
    char *item, *key;
    if (argc < 7) {
      fprintf(stderr, "Not enough arguments. Syntax: %s $ snapshot get %s item key\n", argv[0], snapshot_name);
      return -1;
    }
    item = argv[5];
    key = argv[6];
    
    snapshot = faup_snapshot_read(snapshot_name);
    if (!snapshot) {
      fprintf(stderr, "Cannot read snapshot: %s\n", snapshot_name);
      return -1;
    }

    snapshot_item = faup_snapshot_item_get(snapshot, item);
    if (!snapshot_item) {
      faup_snapshot_free(snapshot);
      return -1;
    }
    vc = faup_snapshot_value_count_get(snapshot_item, key);
    if (!vc) {
      faup_snapshot_free(snapshot);
      return -1;
    }

    strftime(first_timebuf, sizeof(first_timebuf), "%Y-%m-%d %H:%M:%S %z", localtime(&vc->first_time_seen));
    strftime(last_timebuf, sizeof(last_timebuf), "%Y-%m-%d %H:%M:%S %z", localtime(&vc->last_time_seen));
    fprintf(stdout, "{\"value\": \"%s\", \"count\": %ld, \"first seen\": \"%s\", \"last seen\": \"%s\"}\n", vc->value, vc->count, first_timebuf, last_timebuf);
    
  out:
    faup_snapshot_free(snapshot);
    return 0;
  }

#ifdef FAUP_GTCACA
  if (!strcmp(action, "browse")) {
    faup_snapshot_browser(snapshot_name);
    return 0;
  }
#endif
  
  printf("Unknown action: '%s\'n", action);
  return -1;
}

int faup_handle_shell_modules(int argc, char **argv)
{

#ifdef FAUP_LUA_MODULES
  if (argc < 5) {
    printf("Usage: %s $ modules action\n", argv[0]);
    printf("\nWhere action can be:\n");
    printf("list all      : List all modules\n");
    printf("list enabled  : List enabled modules\n");
    printf("list available: List available modules\n\n");

    printf("enable module_name : Enable the module 'module_name'\n");
    printf("disable module_name: Disable the module 'module_name'\n");
    printf("\n");
    return -1;
  }

  if (!strcmp(argv[3], "list") || !strcmp(argv[3], "l")) {
    if (!strcmp(argv[4], "all")) {
      printf("Modules enabled:\n");
      faup_modules_foreach_filelist(NULL, NULL, shell_modules_list_cb, NULL);
      printf("\n");
      printf("Modules available:\n");
      char *global_available = faup_datadir_get_global_file("modules_available");
      faup_modules_foreach_filelist(NULL, global_available, shell_modules_list_cb, NULL);
      printf("\n");
      free(global_available);
    }
    if (!strcmp(argv[4], "enabled")) {
      printf("\n");
      faup_modules_foreach_filelist(NULL, NULL, shell_modules_list_cb, NULL);
      printf("\n");
    }
    if (!strcmp(argv[4], "available")) {
      char *global_available = faup_datadir_get_global_file("modules_available");
      printf("\n");
      faup_modules_foreach_filelist(NULL, global_available, shell_modules_list_cb, NULL);
      printf("\n");
      free(global_available);
    }
  }

  if (!strcmp(argv[3], "enable") || !strcmp(argv[3], "e")) {
      int retval;
      char *enabled_filename;
      char *symlink_file;
      char *origin_filename;
      char *available_filename;

      retval = asprintf(&enabled_filename, "modules_enabled%s%s", FAUP_OS_DIRSEP, argv[4]);
      symlink_file = faup_datadir_file_to_write(enabled_filename, true);
      if (!symlink_file) {
	// I have no symlink, because there is nowhere I can write. Let's create the homedir path and do it again
	retval = faup_datadir_make_dir_from_home("modules_enabled");
	symlink_file = faup_datadir_file_to_write(enabled_filename, true);
      }

      free(enabled_filename);
      retval = asprintf(&origin_filename, "modules_available%s%s", FAUP_OS_DIRSEP, argv[4]);
      available_filename = faup_datadir_get_global_file(origin_filename);
      free(origin_filename);


      retval = symlink(available_filename, symlink_file);
      if (!retval) {
        printf("Module '%s' enabled with success!\n", argv[4]);
      } else {
        printf("Module '%s' cannot be enabled [symlink failed from %s to %s]: %s\n", argv[4], available_filename, symlink_file, strerror(errno));
      }
      free(available_filename);
      free(symlink_file);
  }

  if (!strcmp(argv[3], "disable") || !strcmp(argv[3], "d")) {
      int retval;
      char *enabled_filename;
      char *symlink_file;

      retval = asprintf(&enabled_filename, "modules_enabled%s%s", FAUP_OS_DIRSEP, argv[4]);
      symlink_file = faup_datadir_get_file(enabled_filename, false);
      free(enabled_filename);

      retval = unlink(symlink_file);
      if (!retval) {
        printf("Module '%s' disabled with success!\n", argv[4]);
      } else {
        printf("Module '%s' cannot be deactivated: %s\n", argv[4], strerror(errno));
      }
      free(symlink_file);
  }

#endif // FAUP_LUA_MODULES

  return 0;
}

int faup_handle_shell(int argc, char **argv)
{
  int ret;
  
  if (argc <= 2) {
    printf("Usage: %s $ shell_command [parameters]\n", argv[0]);
    printf("\nAvailable shell comands: modules snapshot\n");
    printf("\n");
    return -1;
  }

  if (!strcmp(argv[2], "modules")) {
    ret = faup_handle_shell_modules(argc, argv);
  }
  if (!strcmp(argv[2], "snapshot")) {
    ret = faup_handle_shell_snapshot(argc, argv);
  }

  return ret;
}
