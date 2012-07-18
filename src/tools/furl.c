#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

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

/* readline() - read a line from the file handle.
 * Return an allocated string */
static char *readline(FILE *fp)
{
        char *str = (char *)NULL;
        int ch = 0, len = 256, step = 256, i = 0;

        str = (char *)malloc(len);
        if (str == NULL)
                return str;

        while (1) {
                ch = fgetc(fp);
                if (feof(fp)) {
			str[i] = '\0';
                        break;
		}
                if (ch == '\n' || ch == '\r') {
                        str[i++] = 0;
                        break;
                }
                str[i++] = ch;
                if (i == len - 2) {
                        len += step;
                        str = (char *)realloc(str, len);
                        if (str == NULL) {
                                fclose(fp);
                                return str;
                        }
                }
        }
        return str;
}

int main(int argc, char **argv)
{
	furl_handler_t *fh;

	char *strbuf=NULL;

	fh = furl_init();

	if (isatty(fileno(stdin))) {
		if (argc < 2) {
			fprintf(stderr, "%s url\n", argv[0]);
			exit(1);
		}
		furl_decode(fh, argv[1], strlen(argv[1]));
		furl_show(fh, ',', stdout);
		printf("\n");
	} else {		/* We read from stdin */
		while (!feof(stdin)) {
			strbuf = readline(stdin);
			if (!strbuf) {
				break;
			}
			if (strbuf[0] == '\0') {
				break;
			}

			furl_decode(fh, strbuf, strlen(strbuf));			
			furl_show(fh, ',', stdout);
			printf("\n");

			free(strbuf);
		}
	}

	furl_terminate(fh);

	return 0;
}
