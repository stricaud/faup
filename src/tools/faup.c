#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/options.h>
#include <faup/output.h>

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

static int run_from_stream(faup_handler_t *fh, faup_options_t *options, FILE *stream) 
{
		char *strbuf=NULL;

		while (!feof(stream)) {
			strbuf = readline(stream);
			if (!strbuf) {
				break;
			}
			if (strbuf[0] == '\0') {
			        free(strbuf);
				break;
			}

			faup_decode(fh, strbuf, strlen(strbuf), options);

			faup_output(fh, options, stdout);

			options->print_header = 0; // We don't need to output the header anymore!
			free(strbuf);
			options->current_line++;
		}
	return 0;
}

void print_help(char **argv) 
{
	printf("Usage: %s [-plu] [-d delimiter] [-o {csv,json}] [-f {scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment}] url|file\n \
		Where:\n \
		url is the url that you want to parse\n \
		\t-a: skip provided argument file open check\n \
		\t-d delimiter: will separate the fields with the wanted delimiter\n \
		\t-f: fields to extract\n \
		\t-h: print the header\n \
		\t-l: prefix with the line number\n \
		\t-o: output csv or json at your convenience\n \
		\t-p: print the header\n \
		\t-t: Do not extract TLD > 1 (eg. only get 'uk' instead of 'co.uk')\n \
		\t-u: update the mozilla list\n", argv[0]);
}

int main(int argc, char **argv)
{
	faup_handler_t *fh;
	int ret = 0;

	char *tld_file=NULL;

	faup_options_t *faup_opts;
	int opt;
	int skip_file_check = 0;

	int tld_pos;

	faup_opts = faup_options_new();
	if (!faup_opts) {
	  fprintf(stderr, "Error: cannot allocate faup options!\n" );
	  return -1;
	}

	fh = faup_init();

	while ((opt = getopt(argc, argv, "apld:vo:utf:")) != -1) {
	  switch(opt) {
	  case 'a':
	  	skip_file_check = 1;
	  	break;
	  case 'p':
	    faup_opts->print_header = 1;
	    break;
	  case 'l':
	  	faup_opts->fields |= FAUP_URL_FIELD_LINE;
	    faup_opts->print_line = 1;
	    break;
	  case 'd':
	    faup_opts->sep_char = optarg[0];
	    break;
	  case 'f':
	  	if (!strcmp("scheme", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_SCHEME;
	  	}
	  	if (!strcmp("credential", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_CREDENTIAL;
	  	}
	  	if (!strcmp("subdomain", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_SUBDOMAIN;
	  	}
	  	if (!strcmp("domain", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_DOMAIN;
	  	}
	  	if (!strcmp("host", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_HOST;
	  	}
	  	if (!strcmp("tld", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_TLD;
	  	}
	  	if (!strcmp("port", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_PORT;
	  	}
	  	if (!strcmp("resource_path", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_RESOURCE_PATH;
	  	}
	  	if (!strcmp("query_string", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_QUERY_STRING;
	  	}
	  	if (!strcmp("fragment", optarg)) {
	  		faup_opts->fields = FAUP_URL_FIELD_FRAGMENT;
	  	}
	  	break;
	  case 'o':
	  	if (!strcmp("csv", optarg)) {
	  		faup_opts->output = FAUP_OUTPUT_CSV;
	  	} else if (!strcmp("json", optarg)) {
	  		faup_opts->output = FAUP_OUTPUT_JSON;
	  	} else {
	  		fprintf(stderr, "invalid output option '%s'!\n", optarg);
			faup_options_free(faup_opts);
	  		exit(1);
	  	}
	  	break;
	  case 't':
	  	faup_options_disable_tld_above_one(faup_opts);
	  	break;
	  case 'u':
	  	faup_tld_update();
		faup_options_free(faup_opts);
	  	exit(0);
	  	break;
	  case 'v':
	    printf("faup v%s\n", faup_get_version());
	    faup_options_free(faup_opts);
	    exit(0);
	    break;
	  default:
	    print_help(argv);
	    faup_options_free(faup_opts);
	    exit(1);
	  }
	}

	if (isatty(fileno(stdin))) {
		faup_opts->input_source = FAUP_INPUT_SOURCE_ARGUMENT;

		if (argc < 2) {
			print_help(argv);
			faup_options_free(faup_opts);
			exit(1);
		}

		if (!argv[optind]) {
		  faup_options_free(faup_opts);
		  exit(1);
		}

		if (!skip_file_check) {
			if (strlen(argv[optind]) < FAUP_MAXPATHLEN) {
				FILE *fp = fopen(argv[optind], "r");
				if (fp) {
					faup_opts->input_source = FAUP_INPUT_SOURCE_FILE;
					ret = run_from_stream(fh, faup_opts, fp);
					fclose(fp);
					goto terminate;
				}
			}
		}

		faup_decode(fh, argv[optind], strlen(argv[optind]), faup_opts);

		faup_output(fh, faup_opts, stdout);
	} else {       	/* We read from stdin */
		faup_opts->input_source = FAUP_INPUT_SOURCE_PIPE;
		run_from_stream(fh, faup_opts, stdin);
	}

terminate:
	faup_options_free(faup_opts);
	faup_terminate(fh);

	return ret;
}
