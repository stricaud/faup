#include <stdio.h>
#include <furl/furl.h>
#include <furl/decode.h>

/* IPv6 tests:

      http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html
      http://[1080:0:0:0:8:800:200C:417A]/index.html
      http://[3ffe:2a00:100:7031::1]
      http://[1080::8:800:200C:417A]/foo
      http://[::192.9.5.5]/ipng
      http://[::FFFF:129.144.52.38]:80/index.html
      http://[2010:836B:4179::836B:4179]
 */

int main(int argc, char **argv)
{
	furl_handler_t *fh;
	int retval;
	fh = furl_init();

	if (argc < 2) {
		fprintf(stderr, "%s url\n", argv[0]);
	}

	retval = furl_decode(fh, argv[1]);
	if (retval > 0) {
		fprintf(stderr, "There was an error:%d\n", retval);
	}
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
	/* f_url = furl_decode(fh, "http://foo:bar@google.com:324/foo?ref=http://www.slashdot.org"); */

	/* if (!f_url) { */
	/* 	fprintf(stderr, "Cannot decode given url!\n"); */
	/* 	furl_terminate(fh); */
	/* 	return 1; */
	/* } */


	furl_terminate(fh);

	return 0;
}
