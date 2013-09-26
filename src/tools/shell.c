#include <stdio.h>
#include <string.h>

int faup_handle_shell(int argc, char **argv)
{
  if (argc <= 2) {
    printf("faup $ shell_command parameters\n");
    printf("available shell comands: module\n");
    return -1;
  }

  if (!strcmp(argv[2], "module")) {
    if (argc < 5) {
      printf("Module help\n");
      return -1;
    } 

    if (!strcmp(argv[3], "list")) {
      if (!strcmp(argv[4], "enabled")) {
	printf("list enabled plugins\n");
      }
      if (!strcmp(argv[4], "available")) {
	printf("list available plugins\n");

      }
    }

  }

  return 0;
}
