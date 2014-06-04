#include <stdio.h>
#include <string.h>

#include <faup/base64.h>

int main(int argc, char **argv)
{
  unsigned char *data_unbase64;
  char          *data_base64;
  int            outlen;

  if (argc < 2) {
    fprintf(stderr, "No arguments given. Please provide an url to test\n");
    return -1;
  }

    data_base64 = base64( (const void*) argv[1], strlen(argv[1]) + 1, &outlen);
    printf("base64:-->%s<--\n", data_base64);

    outlen = strlen((const char *)data_base64);

  data_unbase64 = unbase64((const char *)data_base64, outlen, &outlen);
  printf("unbase64:-->%s<--\n", data_unbase64);

  outlen = strlen((const char *)data_unbase64);

  free(data_base64);
  free(data_unbase64);


}
