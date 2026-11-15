/*
 * Decode URLs from several threads at once.
 *
 * The faup options are shared; each thread owns its faup handler, since a
 * handler carries the state of the URL being decoded and must not be shared
 * between threads.
 */
#include <stdio.h>
#include <string.h>
#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/output.h>

#include <assert.h>
#include <pthread.h>

#define NUM_THREADS 10
#define NUM_ITERATIONS 100

/*          0         1         2         3
 *          0123456789012345678901234567890123456  */
#define URL "https://www.wallinfire.net/index.html"

void *run_faup(void *argument)
{
	faup_handler_t *fh = (faup_handler_t *)argument;
	int i;

	for (i = 0; i < NUM_ITERATIONS; i++) {
		faup_decode(fh, URL, strlen(URL));
		assert(faup_get_tld_pos(fh) == 23);
		assert(faup_get_tld_size(fh) == 3);
		assert(faup_get_domain_pos(fh) == 12);
		assert(faup_get_domain_size(fh) == 14);
		assert(faup_get_subdomain_pos(fh) == 8);
		assert(faup_get_subdomain_size(fh) == 3);
		assert(faup_get_resource_path_pos(fh) == 26);
	}

	return NULL;
}

int main(int argc, char **argv)
{
	faup_options_t *options;
	faup_handler_t *handlers[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	int i;
	int retval;

	options = faup_options_new();
	if (!options) {
		fprintf(stderr, "Error: cannot allocate faup options!\n");
		return 1;
	}

	for (i = 0; i < NUM_THREADS; i++) {
		handlers[i] = faup_init(options);
		assert(handlers[i]);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		retval = pthread_create(&threads[i], NULL, run_faup, handlers[i]);
		assert(!retval);
	}

	for (i = 0; i < NUM_THREADS; ++i) {
		retval = pthread_join(threads[i], NULL);
		assert(!retval);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		faup_terminate(handlers[i]);
	}
	faup_options_free(options);

	printf("%d threads x %d decodes OK\n", NUM_THREADS, NUM_ITERATIONS);
	return 0;
}
