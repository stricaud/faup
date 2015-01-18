
#ifdef LINUX
#define _POSIX_SOURCE
#define _BSD_SOURCE
#endif

#ifdef MACOS // Workaround daemon(): warning: 'daemon' is deprecated: first deprecated in Mac OS X 10.5 [-Wdeprecated-declarations]
#define daemon sure_it_is_deprecated_for_non_portable_stuff
#endif

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
#include <faup/webserver.h>

#include "shell.h"

#if MACOS
#undef daemon
extern int daemon(int, int);
#endif

faup_handler_t *fh = NULL;
faup_options_t *faup_opts = NULL;
static char *faup_webserver_buffer = NULL;

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
	printf("-b\tRun the webserver in background\n");	
	printf("-d {delimiter}\n\twill separate the fields with the wanted delimiter\n");
	printf("-f {scheme|credential|subdomain|domain|domain_without_tld|host|tld|port|resource_path|query_string|fragment}\n\tfield to extract\n");
	printf("-l\tprefix with the line number (csv only)\n");
	printf("-m\t{module1 module2 module3}\n\tLoad the modules in the given order.\n\tIf empty, load no modules. If this option is not provided, modules in the shared data in modules_enabled will be loaded by default\n");
	printf("-o {csv,json,module}\n\toutput csv or json at your convenience. You can also just let the modules handle it.\n");
	printf("-p\tprint the header\n");
	printf("-r {N}\tremoves the last N chars from the url\n");
	printf("-t\tdo not extract TLD > 1 (eg. only get 'uk' instead of 'co.uk')\n");
	printf("-u\tupdate the mozilla list\n");
	printf("-w listen_ip:port\n\tstarts webserver on the wanted ip:port\n");
	printf("\n");
}

void sighandler_webserver(int signal)
{
  faup_webserver_stop();
  faup_options_free(faup_opts);
  faup_terminate(fh);
  free(faup_webserver_buffer);
  printf("Exiting faup webserver\n");
  exit(0);
}

int main(int argc, char **argv)
{
	int ret = 0;

	char *tld_file=NULL;

	int opt;
	int skip_file_check = 0;
	int run_in_background = 0;

	int tld_pos;

  	bool has_module = false;

	faup_opts = faup_options_new();
	if (!faup_opts) {
	  fprintf(stderr, "Error: cannot allocate faup options!\n" );
	  return -1;
	}

	// Faup default output is CSV
	faup_opts->output = FAUP_OUTPUT_CSV;


	if ((argc > 1) && (!strcmp(argv[1],"$"))) {
	  // Handle some specific shell operations
	  return faup_handle_shell(argc, argv);
	}

	while ((opt = getopt(argc, argv, "abpld:vo:utf:m:w:r:")) != -1) {
	  switch(opt) {
	  case 'a':
	  	skip_file_check = 1;
	  	break;
	  case 'b':
	  	run_in_background = 1;
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
	  	faup_opts->output = faup_options_get_output_from_name(optarg);

	  	if (faup_opts->output == FAUP_OUTPUT_NONE) {
	  		fprintf(stderr, "invalid output option '%s'!\n", optarg);
			faup_options_free(faup_opts);
	  		exit(1);
	  	}
	  	
	  	break;
	  case 'r':
	  	if (optarg) {
		  faup_opts->number_of_chars_to_remove = strtod(optarg, NULL);
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
	  case 'w': // This should always be the last options, since we init faup and need previous options
		fh = faup_init(faup_opts);
		if (run_in_background) {
			daemon(1,1);
		}

		signal(SIGINT, sighandler_webserver);
		faup_webserver_buffer = faup_output_json_buffer_allocate();
	  	faup_webserver_start(fh, faup_opts, optarg, faup_webserver_buffer);
	  	break;
	  default:
	    print_help(argv);
	    faup_options_free(faup_opts);
	    exit(1);
	  }
	}

	if (!fh) {
		fh = faup_init(faup_opts);
	}

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
