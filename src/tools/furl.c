#include <stdio.h>
#include <furl/furl.h>
#include <furl/decode.h>

int main(int argc, char **argv)
{
	furl_handler_t *fh;
	furl_t *f_url;

	fh = furl_init();

	if (argc < 2) {
		fprintf(stderr, "%s url\n", argv[0]);
	}

	f_url = furl_decode(fh, argv[1]);
	/* f_url = furl_decode(fh, "www.honeynet.org"); */
	/* f_url = furl_decode(fh, "://www.honeynet.org/"); */
	/* f_url = furl_decode(fh, "localhost"); */
	/* f_url = furl_decode(fh, "127.0.0.1"); */
	/* f_url = furl_decode(fh, "127.0.0.1:8080/toto.html"); */
	/* f_url = furl_decode(fh, "foo@bar:www.slashdot.org:22/fun"); */
	/* f_url = furl_decode(fh, "http://foo@bar:www.slashdot.org"); */
	/* f_url = furl_decode(fh, "http://foo@bar:www.slashdot.org:9191/query?foo=bar#tada"); */
	/* f_url = furl_decode(fh, "foo@bar:www.slashdot.org/tagada@blah"); */
	/* f_url = furl_decode(fh, "http://www.slashdot.org/funny@one"); */
	/* f_url = furl_decode(fh, "http://www.slashdot.org/?blah&ref=http://www.faizbook.org"); */

	/* if (!f_url) { */
	/* 	fprintf(stderr, "Cannot decode given url!\n"); */
	/* 	furl_terminate(fh); */
	/* 	return 1; */
	/* } */


	furl_terminate(fh);

	return 0;
}
