#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>

#include <faup/faup.h>
#include <faup/decode.h>

struct _faup_cli_options_t {
  int print_header;
  char sep_char;
};
typedef struct _faup_cli_options_t faup_cli_options_t;

/* IPv6 tests:

      http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html
      http://[1080:0:0:0:8:800:200C:417A]/index.html
      http://[3ffe:2a00:100:7031::1]
      http://[1080::8:800:200C:417A]/foo
      http://[::192.9.5.5]/ipng
      http://[::FFFF:129.144.52.38]:80/index.html
      http://[2010:836B:4179::836B:4179]
 */

void init_cli_options(faup_cli_options_t *opts)
{
  opts->print_header = 0;
  opts->sep_char = ',';
}

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

void print_help(char **argv) 
{
	printf("Usage: %s [-p] [-d delimiter] url\n \
		Where:\n \
		url is the url that you want to parse\n \
		\t-p: print the header\n \
		\t-d delimiter: will separate the fields with the wanted delimiter\n", argv[0]);
}

void print_header(char sep_char)
{
	printf("scheme%ccredential%csubdomain%cdomain%chost%ctld%cport%cresource_path%cquery_string%cfragment\n",
	       sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char);
}

int main(int argc, char **argv)
{
	faup_handler_t *fh;
	char *strbuf=NULL;

	faup_cli_options_t faup_opts;
	int opt;

	init_cli_options(&faup_opts);

	fh = faup_init();

	while ((opt = getopt(argc, argv, "pd:v")) != -1) {
	  switch(opt) {
	  case 'p':
	    faup_opts.print_header = 1;
	    break;
	  case 'd':
	    faup_opts.sep_char = optarg[0];
	    break;
	  case 'v':
	    printf("faup v%s\n", faup_get_version());
	    exit(0);
	  default:
	    print_help(argv);
	    exit(1);
	  }
	}

	if (isatty(fileno(stdin))) {
		if (argc < 2) {
			print_help(argv);
			exit(1);
		}
		if (faup_opts.print_header) {
			print_header(faup_opts.sep_char);
		}

		if (!argv[optind]) {
		  exit(1);
		}

		faup_decode(fh, argv[optind], strlen(argv[optind]));
		faup_show(fh, faup_opts.sep_char, stdout);
		printf("\n");
	} else {		/* We read from stdin */
		if (faup_opts.print_header) {
			print_header(faup_opts.sep_char);
		}
		while (!feof(stdin)) {
			strbuf = readline(stdin);
			if (!strbuf) {
				break;
			}
			if (strbuf[0] == '\0') {
				break;
			}

			faup_decode(fh, strbuf, strlen(strbuf));			
			faup_show(fh, ',', stdout);
			printf("\n");

			free(strbuf);
		}
	}

	faup_terminate(fh);

	return 0;
}
