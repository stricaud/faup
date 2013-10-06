
#ifdef LINUX
#define _POSIX_SOURCE
#define _BSD_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#ifdef WIN32
	#include "win32-getopt.h"
#else
	#include <getopt.h>
#endif

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/options.h>
#include <faup/output.h>


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

static int run_from_stream(faup_handler_t *fh, FILE *stream) 
{
		char *strbuf=NULL;

		while (!feof(stream)) {
			strbuf = readline(stream);
			if (!strbuf) {
				break;
			}
			if (strbuf[0] == '\0') {
			    free(strbuf);
			    // We can have an empty line but other after (see #27)
				continue;
			}

			faup_decode(fh, strbuf, strlen(strbuf));

			faup_output(fh, stdout);

			fh->options->print_header = 0; // We don't need to output the header anymore!
			free(strbuf);
			fh->options->current_line++;
		}
	return 0;
}


void print_help(char **argv)
{
	printf("\nUsage: %s [options] url|file\n\n", argv[0]);

	printf("Options:\n");
	printf("-a\tskip provided argument file open check\n");
	printf("-d {delimiter}\n\twill separate the fields with the wanted delimiter\n");
	printf("-f {scheme|credential|subdomain|domain|domain_without_tld|host|tld|port|resource_path|query_string|fragment}\n\tfield to extract\n");
	printf("-h\tprint the header\n");
	printf("-l\tprefix with the line number (csv only)\n");
	printf("-m\t{module1 module2 module3}\n\tLoad the modules in the given order.\n\tIf empty, load no modules. If this option is not provided, modules in the shared data in modules_enabled will be loaded by default\n");
	printf("-o {csv,json,module}\n\toutput csv or json at your convenience. You can also just let the modules handle it.\n");
	printf("-p\tprint the header\n");
	printf("-t\tdo not extract TLD > 1 (eg. only get 'uk' instead of 'co.uk')\n");
	printf("-u\tupdate the mozilla list\n");
	printf("\n");
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

  	bool has_module = false;


	faup_opts = faup_options_new();
	if (!faup_opts) {
	  fprintf(stderr, "Error: cannot allocate faup options!\n" );
	  return -1;
	}

	if ((argc > 1) && (!strcmp(argv[1],"$"))) {
	  // Handle some specific shell operations
	  return faup_handle_shell(argc, argv);
	}

	while ((opt = getopt(argc, argv, "ape:ld:vo:utf:m:")) != -1) {
	  switch(opt) {
	  case 'a':
	  	skip_file_check = 1;
	  	break;
	  case 'd':
	  	if (optarg) {
	    	faup_opts->sep_char = optarg[0];
	    }
	    break;	 
	  case 'p':
	    faup_opts->print_header = 1;
	    break;
	  case 'l':
	  	faup_opts->fields |= FAUP_URL_FIELD_LINE;
	    faup_opts->print_line = 1;
	    break;
	  case 'm':
	  	for (optind--;optind < argc - 1;) {
	  		// In case we parse the next option, we shall stop!
	  		if (argv[optind][0] == '-') {
	  			break;
	  		} 

	  		optind++;

	  		if (!has_module) {
	  			// This is our first argument
	  			// we push our first arg: 'optarg'
	  			faup_opts->modules_argv = realloc(faup_opts->modules_argv, faup_opts->modules_argc + 1);
	  			faup_opts->modules_argv[faup_opts->modules_argc] = strdup(optarg);

	  			faup_opts->modules_argc++;
	  			has_module = 1;
	  		}
	  		

	  		// In case we parse the next option, we shall stop!
	  		// In case the next argument is the last, that means this is the url, not the module
	  		if ((argv[optind][0] == '-') | (optind == argc - 1)) {
	  			break;
	  		}

	  		faup_opts->modules_argv = realloc(faup_opts->modules_argv, faup_opts->modules_argc + 1);
	  		faup_opts->modules_argv[faup_opts->modules_argc] = strdup(argv[optind]);
	  		faup_opts->modules_argc++;
	  	}

	  	if (has_module) {
  			faup_opts->exec_modules = FAUP_MODULES_EXECARG;
  		} else {
  			faup_opts->exec_modules = FAUP_MODULES_NOEXEC;
  		}
	  	break;
	  case 'f':
	  	faup_opts->fields = faup_options_get_field_from_name(optarg);
	  	break;
	  case 'o':
	  	if (!strcmp("csv", optarg)) {
	  		faup_opts->output = FAUP_OUTPUT_CSV;
	  	} else if (!strcmp("json", optarg)) {
	  		faup_opts->output = FAUP_OUTPUT_JSON;
	  	} else if (!strcmp("module", optarg)) {
	  		faup_opts->output = FAUP_OUTPUT_MODULE;
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
	    faup_tld_datadir_print();
	    faup_options_free(faup_opts);
	    exit(0);
	    break;
	  default:
	    print_help(argv);
	    faup_options_free(faup_opts);
	    exit(1);
	  }
	}

	fh = faup_init(faup_opts);

//	printf("Lart arg:'%s'\n", argv[optind+1]);


	if (isatty(fileno(stdin))) {
		faup_opts->input_source = FAUP_INPUT_SOURCE_ARGUMENT;

		if (argc < 2) {
			print_help(argv);
			faup_options_free(faup_opts);
			faup_terminate(fh);
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
					ret = run_from_stream(fh, fp);
					fclose(fp);
					goto terminate;
				}
			}
		}

		faup_decode(fh, argv[optind], strlen(argv[optind]));

		faup_output(fh, stdout);
	} else {       	/* We read from stdin */
		faup_opts->input_source = FAUP_INPUT_SOURCE_PIPE;
		run_from_stream(fh, stdin);
	}

terminate:
	faup_options_free(faup_opts);
	faup_terminate(fh);

	return ret;
}
