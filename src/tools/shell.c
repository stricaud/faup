#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <faup/datadir.h>
#include <faup/modules.h>


int shell_modules_list_enabled_cb(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
  printf("[%d] %s%s%s\n", count, modules_dir, FAUP_OS_DIRSEP, module);

  return 0;
}


int faup_handle_shell_modules(int argc, char **argv)
{

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
      faup_modules_foreach_filelist(NULL, NULL, shell_modules_list_enabled_cb, NULL);
      printf("\n");
      printf("Modules available:\n");
      char *global_available = faup_datadir_get_global_file("modules_available");
      faup_modules_foreach_filelist(NULL, global_available, shell_modules_list_enabled_cb, NULL);
      printf("\n");
      free(global_available);
    }
    if (!strcmp(argv[4], "enabled")) {
      printf("\n");
      faup_modules_foreach_filelist(NULL, NULL, shell_modules_list_enabled_cb, NULL);
      printf("\n");
    }
    if (!strcmp(argv[4], "available")) {
      char *global_available = faup_datadir_get_global_file("modules_available");
      printf("\n");
      faup_modules_foreach_filelist(NULL, global_available, shell_modules_list_enabled_cb, NULL);
      printf("\n");
      free(global_available);
    }
  }

  if (!strcmp(argv[3], "enable") || !strcmp(argv[3], "e")) {
    printf("enable\n");
  }

  if (!strcmp(argv[3], "disable") || !strcmp(argv[3], "d")) {
    printf("disable\n");
  }

  return 0;
}

int faup_handle_shell(int argc, char **argv)
{
  if (argc <= 2) {
    printf("Usage: %s $ shell_command [parameters]\n", argv[0]);
    printf("\nAvailable shell comands: module\n");
    printf("\n");
    return -1;
  }

  if (!strcmp(argv[2], "modules")) {
    faup_handle_shell_modules(argc, argv);
  }

  return 0;
}
