
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
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>

#ifdef WIN32
	#include "win32-getopt.h"
#else
	#include <getopt.h>
#endif

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/options.h>
#include <faup/output.h>
#include <faup/snapshot.h>
#include <faup/snapshot-file.h>
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
		if (ch == EOF) {
			/* End of file, or a read error (e.g. the "file" is a directory):
			 * feof() alone would spin forever on the latter. */
			str[i] = '\0';
			break;
		}
		if (ch == '\n' || ch == '\r') {
			str[i++] = 0;
			break;
		}
		str[i++] = ch;
		if (i == len - 2) {
			char *newstr;
			len += step;
			newstr = (char *)realloc(str, len);
			if (newstr == NULL) {
				free(str);
				return NULL;
			}
			str = newstr;
		}
	}

	return str;
}

static int run_from_stream(faup_handler_t *fh, FILE *stream) 
{
		char *strbuf=NULL;
		faup_snapshot_t *snapshot = NULL;
		char *value;
		
		if (fh->options->snapshot_name) {
		  snapshot = faup_snapshot_open(fh->options->snapshot_name);
		  if (!snapshot) {
		    fprintf(stderr, "Cannot open the snapshot '%s'\n", fh->options->snapshot_name);
		    return -1;
		  }
		}
		
		while (!feof(stream) && !ferror(stream)) {
			strbuf = readline(stream);
			if (!strbuf) {
				break;
			}
			if (!fh->options->empty_lines_processed) {
			  if (strbuf[0] == '\0') {
			    free(strbuf);
			    // We can have an empty line but other after (see #27)
			    continue;
			  }
			}
			faup_decode(fh, strbuf, strlen(strbuf));

			if (fh->options->snapshot_name) {
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_SCHEME);
			  if (value) {
			    faup_snapshot_append(snapshot, "scheme", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_HIERARCHICAL);
			  if (value) {
			    faup_snapshot_append(snapshot, "herarchical", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_SUBDOMAIN);
			  if (value) {
			    faup_snapshot_append(snapshot, "subdomain", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_DOMAIN);
			  if (value) {
			    faup_snapshot_append(snapshot, "domain", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_DOMAIN_WITHOUT_TLD);
			  if (value) {
			    faup_snapshot_append(snapshot, "domain_without_tld", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_HOST);
			  if (value) {
			    faup_snapshot_append(snapshot, "host", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_TLD);
			  if (value) {
			    faup_snapshot_append(snapshot, "tld", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_PORT);
			  if (value) {
			    faup_snapshot_append(snapshot, "port", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_RESOURCE_PATH);
			  if (value) {
			    faup_snapshot_append(snapshot, "resource_path", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_QUERY_STRING);
			  if (value) {
			    faup_snapshot_append(snapshot, "query_string", value);
			    free(value);
			  }
			  value = faup_features_get_string(fh, FAUP_FEATURES_FIELD_FRAGMENT);
			  if (value) {
			    faup_snapshot_append(snapshot, "fragment", value);
			    free(value);
			  }
			} // if (fh->options->snapshot_name) {
			
			faup_output(fh, stdout);

			fh->options->print_header = 0; // We don't need to output the header anymore!
			free(strbuf);
			fh->options->current_line++;
		}

		if (fh->options->snapshot_name) {
		  faup_snapshot_write(snapshot, NULL);
		  faup_snapshot_free(snapshot);
		}
		
		return 0;
}


/* Append a module name to the -m list. The list is an array of pointers. */
static int push_module_arg(faup_options_t *opts, const char *name)
{
	char **newargv;
	char *copy;

	newargv = realloc(opts->modules_argv, sizeof(char *) * (opts->modules_argc + 1));
	if (!newargv) {
		fprintf(stderr, "Error: cannot allocate the modules list\n");
		return -1;
	}
	opts->modules_argv = newargv;
	copy = strdup(name);
	if (!copy) {
		fprintf(stderr, "Error: cannot allocate the module name\n");
		return -1;
	}
	opts->modules_argv[opts->modules_argc] = copy;
	opts->modules_argc++;

	return 0;
}

void print_help(char **argv)
{
	printf("\nUsage: %s [options] url|file\n\n", argv[0]);

	printf("Options:\n");
	printf("-a\tskip provided argument file open check\n");
	printf("-b\tRun the webserver in background\n");	
	printf("-c <snapshot>\tcompare with snapshot selected with -s\n");
	printf("-d {delimiter}\n\twill separate the fields with the wanted delimiter\n");
	printf("-e\tempty lines are processed\n");	
	printf("-f {scheme|credential|subdomain|domain|domain_without_tld|host|tld|port|resource_path|query_string|fragment|url_type}\n\tfield to extract\n");
	printf("-h\tprint this help\n");
	printf("-l\tprefix with the line number (csv only)\n");
#ifdef FAUP_LUA_MODULES
	printf("-m\t{module1.lua module2.lua ...}\n\tLoad the modules in the given order (a module that cannot be found is an error).\n\tThe last argument is the url or file to parse unless it ends with .lua. If this option is not provided, modules in the shared data in modules_enabled will be loaded by default\n");
#endif
	printf("-o {csv,json,module}\n\toutput csv or json at your convenience. You can also just let the modules handle it.\n");
	printf("-p\tprint the header\n");
	printf("-q\tquiet mode, do not print anything\n");
	printf("-r {N}\tremoves the last N chars from the url\n");
	printf("-s <snapshot_name>\tcreate a snapshot\n");
	printf("-t\tdo not extract TLD > 1 (eg. only get 'uk' instead of 'co.uk')\n");
	printf("-u\tupdate the mozilla list\n");
	printf("-v\tprint the program version, data dirs and compiled options\n");
	printf("-w listen_ip:port\n\tstarts webserver on the wanted ip:port\n");
	printf("\n");
}

void sighandler_webserver(int signal)
{
#ifdef FAUP_WEBSERVER
  faup_webserver_stop();
#endif
  faup_options_free(faup_opts);
  faup_terminate(fh);
#ifdef FAUP_WEBSERVER
  free(faup_webserver_buffer);
#endif
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

	char *snapshot_compare = NULL;

	if ((argc > 1) && (!strcmp(argv[1],"--cflags"))) {
	  printf(LIB_CFLAGS);
	  if ((argc > 2) && (!strcmp(argv[2],"--libs"))) {
	    printf(" "LIB_LIBS);	    
	  }
	  printf("\n");
	  exit(0);
	}
	if ((argc > 1) && (!strcmp(argv[1],"--libs"))) {
	  printf(LIB_LIBS);
	  if ((argc > 2) && (!strcmp(argv[2],"--cflags"))) {
	    printf(" "LIB_CFLAGS);	    
	  }
	  printf("\n");
	  exit(0);
	}

	
	faup_opts = faup_options_new();
	if (!faup_opts) {
	  fprintf(stderr, "Error: cannot allocate faup options!\n" );
	  return -1;
	}

	// Faup default output is CSV
	faup_opts->output = FAUP_OUTPUT_CSV;
	faup_opts->exec_modules = FAUP_MODULES_EXECPATH;

	if ((argc > 1) && (!strcmp(argv[1],"$"))) {
	  // Handle some specific shell operations
	  return faup_handle_shell(argc, argv);
	}

	while ((opt = getopt(argc, argv, "abepld:vo:utf:hm:w:r:s:c:q")) != -1) {
	  switch(opt) {
	  case 'a':
	  	skip_file_check = 1;
	  	break;
	  case 'b':
	  	run_in_background = 1;
	  	break;
	  case 'c':
	    snapshot_compare = strdup(optarg);
	    break;
	  case 'd':
	  	if (optarg) {
	    	faup_opts->sep_char = optarg[0];
	    }
	    break;	 
	  case 'e':
	    faup_opts->empty_lines_processed = 1;
	    break;
	  case 'f':
	  	faup_opts->fields = faup_options_get_field_from_name(optarg);
	  	break;
	  case 'h':
	    print_help(argv);
	    faup_options_free(faup_opts);
	    exit(1);	  	
	  	break;
	  case 'l':
	    faup_opts->fields |= FAUP_URL_FIELD_LINE;
	    faup_opts->print_line = 1;
	    break;
	  case 'm':
#ifndef FAUP_LUA_MODULES
	  	fprintf(stderr, "Error: this faup was built without Lua support, modules (-m) are not available.\n");
	  	faup_options_free(faup_opts);
	  	exit(1);
#endif
	  	/* Module names follow -m up to the next option. The last non-option argument
	  	 * is the URL or file to parse unless it is itself a module (ends with ".lua"),
	  	 * so that both "faup -m mod.lua url" and "cat urls | faup -m mod.lua" work. */
	  	if (optarg[0] == '-') {
	  		fprintf(stderr, "Error: -m requires a module name\n");
	  		faup_options_free(faup_opts);
	  		exit(1);
	  	}
	  	if (push_module_arg(faup_opts, optarg) < 0) {
	  		exit(1);
	  	}
	  	has_module = 1;
	  	while ((optind < argc) && (argv[optind][0] != '-')) {
	  		size_t arglen = strlen(argv[optind]);
	  		bool is_lua = (arglen > 4) && !strcmp(argv[optind] + arglen - 4, ".lua");
	  		if ((optind == argc - 1) && !is_lua) {
	  			break; /* the URL or file argument */
	  		}
	  		if (push_module_arg(faup_opts, argv[optind]) < 0) {
	  			exit(1);
	  		}
	  		optind++;
	  	}

	  	if (has_module) {
  			faup_opts->exec_modules = FAUP_MODULES_EXECARG;
  		} else {
  			faup_opts->exec_modules = FAUP_MODULES_NOEXEC;
  		}
	  	break;
	  case 'o':
	  	faup_opts->output = faup_options_get_output_from_name(optarg);

	  	if (faup_opts->output == FAUP_OUTPUT_NONE) {
	  		fprintf(stderr, "invalid output option '%s'!\n", optarg);
			faup_options_free(faup_opts);
	  		exit(1);
	  	}
	  	
	  	break;
	  case 'p':
	    faup_opts->print_header = 1;
	    break;
	  case 'q':
	    faup_opts->quiet = 1;
	    break;
	  case 'r':
	  	if (optarg) {
		  char *endptr = NULL;
		  long n = strtol(optarg, &endptr, 10);
		  if ((endptr == optarg) || (*endptr != '\0') || (n < 0) || (n > INT_MAX)) {
		    fprintf(stderr, "Invalid number of chars to remove: '%s'\n", optarg);
		    faup_options_free(faup_opts);
		    exit(1);
		  }
		  faup_opts->number_of_chars_to_remove = (int)n;
		}
		break;
	  case 's':
	  	if (optarg) {
		  faup_opts->snapshot_name = strdup(optarg);
		}
		break;	    
	  case 't':
	  	faup_options_disable_tld_above_one(faup_opts);
	  	break;
	  case 'u':
	  	ret = faup_tld_update();
		faup_options_free(faup_opts);
	  	exit(ret == 0 ? 0 : 1);
	  	break;
	  case 'v':
	    printf("faup v%s\n", faup_get_version());
	    faup_tld_datadir_print();
	    printf("compiled with: ");
#ifdef FAUP_LUA_MODULES
	    printf("lua ");
#endif 	    
#ifdef FAUP_DEBUG
	    printf("debug ");
#endif 	    
#ifdef FAUP_GTCACA
	    printf("gtcaca ");
#endif 	    
#ifdef MACOS
	    printf("macos ");
#endif 	    
#ifdef WIN32
	    printf("win32 ");
#endif 	    
#ifdef LINUX
	    printf("linux ");
#endif
#ifdef FAUP_WEBSERVER
	    printf("webserver ");
#endif
	    printf("\n");	    
	    faup_options_free(faup_opts);
	    exit(0);
	    break;
	  case 'w': // This should always be the last options, since we init faup and need previous options
#ifdef FAUP_WEBSERVER	    
		fh = faup_init(faup_opts);
		if (run_in_background) {
			ret = daemon(1,1);
			if (ret != 0) {
			  fprintf(stderr, "Error starting daemon mode:%s\n", strerror(errno));
			  exit(ret);
			}
		}

		signal(SIGINT, sighandler_webserver);
		faup_webserver_buffer = faup_output_json_buffer_allocate();
	  	faup_webserver_start(fh, faup_opts, optarg, faup_webserver_buffer);
#endif
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
	if (!fh) {
		fprintf(stderr, "Error: cannot initialize faup\n");
		faup_options_free(faup_opts);
		exit(1);
	}
#ifdef FAUP_LUA_MODULES
	if ((faup_opts->exec_modules == FAUP_MODULES_EXECARG) && !fh->modules) {
		// The requested module(s) could not be loaded (issue #90)
		faup_options_free(faup_opts);
		faup_terminate(fh);
		exit(1);
	}
#endif

//	printf("Lart arg:'%s'\n", argv[optind+1]);

	/* Snapshot Comparison */
	if (snapshot_compare) {
	  faup_snapshot_t *result = faup_snapshot_compare(faup_opts->snapshot_name, snapshot_compare);
	  faup_snapshot_output(fh, result, stdout);
	  free(snapshot_compare);
	  faup_snapshot_free(result);
	  goto terminate;
	}

	if (optind < argc) {
		/* A URL or a file name was given as argument. It takes precedence
		 * over stdin, so faup behaves the same in a terminal, in a pipe
		 * and under a test runner. */
		faup_opts->input_source = FAUP_INPUT_SOURCE_ARGUMENT;

		if (!skip_file_check) {
			if (strlen(argv[optind]) < FAUP_MAXPATHLEN) {
				FILE *fp = fopen(argv[optind], "r");
				if (fp) {
					struct stat st;
					if ((fstat(fileno(fp), &st) == 0) && S_ISDIR(st.st_mode)) {
						fprintf(stderr, "Error: '%s' is a directory\n", argv[optind]);
						fclose(fp);
						ret = 1;
						goto terminate;
					}
					faup_opts->input_source = FAUP_INPUT_SOURCE_FILE;
					ret = run_from_stream(fh, fp);
					fclose(fp);
					goto terminate;
				}
			}
		}

		faup_decode(fh, argv[optind], strlen(argv[optind]));
		
		faup_output(fh, stdout);
	} else if (!isatty(fileno(stdin))) {
		/* No argument: we read from stdin */
		faup_opts->input_source = FAUP_INPUT_SOURCE_PIPE;
		run_from_stream(fh, stdin);
	} else {
		/* Nothing to parse */
		print_help(argv);
		faup_options_free(faup_opts);
		faup_terminate(fh);
		exit(1);
	}

terminate:
	faup_options_free(faup_opts);
	faup_terminate(fh);

	return ret;
}
