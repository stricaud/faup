#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/syscall.h>

#include <faup/datadir.h>
#include <faup/modules.h>


int shell_modules_list_cb(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
  printf("[%d] %s%s%s\n", count, modules_dir, FAUP_OS_DIRSEP, module);

  return 0;
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
        printf("Module '%s' cannot be enabled: %s\n", argv[4], strerror(errno));
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
  if (argc <= 2) {
    printf("Usage: %s $ shell_command [parameters]\n", argv[0]);
    printf("\nAvailable shell comands: modules\n");
    printf("\n");
    return -1;
  }

  if (!strcmp(argv[2], "modules")) {
    faup_handle_shell_modules(argc, argv);
  }

  return 0;
}
