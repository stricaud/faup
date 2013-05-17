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

void print_help(char **argv) 
{
	printf("Usage: %s [-pl] [-d delimiter] [-o {csv,json}] url\n \
		Where:\n \
		url is the url that you want to parse\n \
		\t-h: print the header\n \
		\t-l: prefix with the line number\n \
		\t-o: output csv or json at your convenience\n \
		\t-d delimiter: will separate the fields with the wanted delimiter\n \
		\t-t: extract TLD > 1\n", argv[0]);
}

void print_header(int print_line, char sep_char)
{
	if (print_line) {
	  printf("line%cscheme%ccredential%csubdomain%cdomain%chost%ctld%cport%cresource_path%cquery_string%cfragment\n",
		 sep_char, sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char);
	} else {
	  printf("scheme%ccredential%csubdomain%cdomain%chost%ctld%cport%cresource_path%cquery_string%cfragment\n",
		 sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char,sep_char);
	}

}

int main(int argc, char **argv)
{
	faup_handler_t *fh;
	char *strbuf=NULL;

	char *tld_file=NULL;

	faup_options_t faup_opts;
	int opt;

	int tld_pos;

	faup_options_defaults(&faup_opts);

	fh = faup_init();


//	tld_tree = faup_tld_tree_new();
//	tld_pos = get_tld_pos(tld_tree, argv[1]);
//	printf("TLD Pos:%d\n", tld_pos);

	while ((opt = getopt(argc, argv, "pld:vo:ut")) != -1) {
	  switch(opt) {
	  case 'p':
	    faup_opts.print_header = 1;
	    break;
	  case 'l':
	    faup_opts.print_line = 1;
	    break;
	  case 'd':
	    faup_opts.sep_char = optarg[0];
	    break;
	  case 'o':
	  	if (!strcmp("csv", optarg)) {
	  		faup_opts.output = FAUP_OUTPUT_CSV;
	  	} else if (!strcmp("json", optarg)) {
	  		faup_opts.output = FAUP_OUTPUT_JSON;
	  	} else {
	  		fprintf(stderr, "invalid output option '%s'!\n", optarg);
	  		exit(1);
	  	}
	  	break;
	  case 't':
	  	faup_opts.tld_tree = faup_tld_tree_new();
	  	faup_opts.tld_greater_extraction = 1;
	  	break;
	  case 'u':
	  	faup_tld_update();
	  	exit(0);
	  	break;
	  case 'v':
	    printf("faup v%s\n", faup_get_version());
	    exit(0);
	    break;
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
		        print_header(faup_opts.print_header, faup_opts.sep_char);
		}

		if (!argv[optind]) {
		  exit(1);
		}

		faup_decode(fh, argv[optind], strlen(argv[optind]), &faup_opts);

		switch(faup_opts.output) {
			case FAUP_OUTPUT_JSON:
				faup_output_json(fh, stdout);
				break;
			default:
				if (faup_opts.print_line) {
		 			printf("0%c", faup_opts.sep_char);
				}
				faup_output_csv(fh, faup_opts.sep_char, stdout);
				printf("\n");
				break;			
		}
	} else {       	/* We read from stdin */
		long line_nb = 1;
		if (faup_opts.print_header) {
		        print_header(faup_opts.print_line, faup_opts.sep_char);
		}
		while (!feof(stdin)) {
			strbuf = readline(stdin);
			if (!strbuf) {
				break;
			}
			if (strbuf[0] == '\0') {
				break;
			}

			faup_decode(fh, strbuf, strlen(strbuf), &faup_opts);		
			switch(faup_opts.output) {
				case FAUP_OUTPUT_JSON:
					faup_output_json(fh, stdout);
					break;
				default:
					if (faup_opts.print_line) {
				        printf("%ld%c", line_nb, faup_opts.sep_char);
					}
					faup_output_csv(fh, ',', stdout);
					printf("\n");
					break;
			}	

			free(strbuf);
			line_nb++;
		}
	}

	faup_terminate(fh);

	return 0;
}
