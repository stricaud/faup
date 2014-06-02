#include <faup/faup.h>

int main(void)
{
  faup_handler_t *fh = NULL;
  faup_options_t *faup_opts;

  faup_opts = faup_options_new();
  fh = faup_init(faup_opts);

  faup_opts = faup_options_new();
  fh = faup_init(faup_opts);

  faup_options_free(faup_opts);

  faup_terminate(fh);

  return 0;
}
