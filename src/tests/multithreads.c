#include <stdio.h>
#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/output.h>

#include <assert.h>
#include <pthread.h>

#define NUM_THREADS 10

void *run_faup(void *argument)
{
       faup_handler_t *fh = (faup_handler_t *)argument;
       int tld_pos;
       int tld_size;

       faup_decode(fh, "https://wallinfire.net", strlen("https://wallinfire.net"));
       tld_pos = faup_get_tld_pos(fh); /* will return 19 */       
       tld_size = faup_get_tld_size(fh); /* will return 3 */       
       faup_output(fh, stdout);

       return NULL;
}

int main(int argc, char **argv)
{
       faup_handler_t *fh;
       faup_options_t *options;

       pthread_t threads[NUM_THREADS];
       int i;
       int retval;

       
       options = faup_options_new();
       if (!options) {
       	  fprintf(stderr, "Error: cannot allocate faup options!\n");
       }
       fh = faup_init(options);

       for (i=0; i<NUM_THREADS; i++) {
	 retval = pthread_create(&threads[i], NULL, run_faup, fh);
	 assert(!retval);
       }

       for (i=0; i<NUM_THREADS; ++i) {
	 retval = pthread_join(threads[i], NULL);
	 assert(!retval);
       }

       faup_terminate(fh);

       return 0;
}
