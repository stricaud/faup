#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <sys/mman.h>
#include <sys/fcntl.h> // For O_RDWR
#include <stdlib.h>
#include <pthread.h>

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/b64/cdecode.h>
#include <faup/output.h>

#include <faup/webserver.h>


static struct mg_context *ctx;
static faup_handler_t *_fh;
static faup_options_t *_faup_opts;
pthread_mutex_t lock;

int log_message(const struct mg_connection *conn, const char *message)
{
    (void) conn;
    printf("%s\n", message);
    return 0;
}

int root_handler(struct mg_connection *conn, void *cbdata)
{

	mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
	mg_printf(conn, " \
		<html> \
		<head><title>Faup webserver</title></head> \
		<body> \
		<h1>Faup webserver</h1> \
		<p> \
		No URL given. Please use the uri such as /json?url=base64_of_your_url \
		</p> \
		</body> \
		</html>");

	return 1;
}

int json_output(struct mg_connection *conn, void *buffer)
{
    char *url_unbase64;
    base64_decodestate s;

    const struct mg_request_info *ri = mg_get_request_info(conn);

    char url[FAUP_MAXLEN];
    size_t url_len;
    int url_outlen;

    pthread_mutex_lock(&lock);

    if (!ri->query_string) {
	    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	    mg_printf(conn, "No URL given. Please use the uri such as /json?url=base64_of_your_url\n");
	    return 1;
    }
    int ret = mg_get_var(ri->query_string, strlen(ri->query_string), "url", url, FAUP_MAXLEN);
    if (ret < 0) {
	    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	    mg_printf(conn, "No URL given. Please use the uri such as /json?url=base64_of_your_url\n");
	    return 1;
    }

    url_len = strlen(url);
    if (url_len < 1) {
    	fprintf(stderr, "ERROR: Invalid given URL of size:%zd\n", url_len);
    	return 1;
    }

    base64_init_decodestate(&s);
    url_unbase64 = (char*)malloc(FAUP_MAX_JSON_BUFFER_SIZE);

    url_outlen = base64_decode_block(url, url_len, url_unbase64, &s);

    faup_decode(_fh, (const char *)url_unbase64, url_outlen);

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

    faup_output_json_buffer(_fh, _faup_opts, buffer);

    mg_printf(conn, "%s", (char *)buffer);

    free(url_unbase64);

    pthread_mutex_unlock(&lock);

    return 1;
}

int snapshot_handler(struct mg_connection *conn, void *cbdata)
{
  
    pthread_mutex_lock(&lock);
    
    pthread_mutex_unlock(&lock);
}

int faup_webserver_start(faup_handler_t *fh, faup_options_t *faup_opts, char *listening_ports, char *buffer)
{
	const char *options[] = {"document_root", ".", 
				 "listening_ports", listening_ports, 
				 NULL};

	_faup_opts = faup_opts;
	if (!faup_opts) {
	  fprintf(stderr, "Error: cannot allocate faup options!\n" );
	  return 1;
	}

	faup_opts->output = FAUP_OUTPUT_JSON;
	_fh = fh;


	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.log_message = &log_message;
	// callbacks.begin_request = &begin_request_handler;

	ctx = mg_start(&callbacks, NULL, options);

	mg_set_request_handler(ctx, "/json", json_output, buffer);
	mg_set_request_handler(ctx, "/", root_handler, NULL);
	mg_set_request_handler(ctx, "/snapshot", snapshot_handler, NULL);
	
	while (1) {
		#ifdef WIN32
			Sleep(1000);
		#else
			sleep(1);
		#endif
	}

	return 0;
}

int faup_webserver_stop()
{
	mg_stop(ctx);

	return 0;
}

