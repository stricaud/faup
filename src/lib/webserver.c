#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <sys/mman.h>
#include <sys/fcntl.h> // For O_RDWR

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/base64.h>
#include <faup/output.h>

#include <faup/webserver.h>

// static void *callbacks(enum mg_event event, struct mg_connection *conn)
// {
// 	const struct mg_request_info *ri = mg_get_request_info(conn);
// 	if (event == MG_NEW_REQUEST) {
// 		printf("We handle a new resquest with ri->url:%s\n", ri->url);
// 	}
// }

static struct mg_context *ctx;
static faup_handler_t *_fh;
static faup_options_t *_faup_opts;


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

int json_output(struct mg_connection *conn, void *cbdata)
{
	unsigned char *url_unbase64;
	static char *buffer = NULL;
	if (!buffer) {
		buffer = faup_output_json_buffer_allocate();
		if (!buffer) {
			return 1;
		}
	}

    const struct mg_request_info *ri = mg_get_request_info(conn);
    // printf("ri->uri=%s\n", ri->uri);

    char url[FAUP_MAXLEN];
    size_t url_len;
    int url_outlen;

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

    url_unbase64 = unbase64((const char *)url, strlen(url), &url_outlen);
    if (url_unbase64[url_outlen - 1] == '\n') {
    	url_unbase64[url_outlen - 1] = '\0';
    }
    // printf("URL unbased:%s\n", url_unbase64);

	faup_decode(_fh, (const char *)url_unbase64, strlen((const char *)url_unbase64));

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

	faup_output_json_buffer(_fh, _faup_opts, buffer);

	mg_printf(conn, buffer);

    // mg_printf(conn, "<html><body>");
    // mg_printf(conn, "<h2>This is the A handler!!!</h2>");
    // mg_printf(conn, "</body></html>\n");

    return 1;
}

int faup_webserver_start(faup_handler_t *fh, faup_options_t *faup_opts, char *listening_ports)
{
	const char *options[] = {"document_root", ".", 
							 "listening_ports", listening_ports, 
							 NULL};


	_faup_opts = faup_opts;
	if (!faup_opts) {
	  fprintf(stderr, "Error: cannot allocate faup options!\n" );
	  return -1;
	}

	faup_opts->output = FAUP_OUTPUT_JSON;
	_fh = fh;


	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.log_message = &log_message;
	// callbacks.begin_request = &begin_request_handler;

	ctx = mg_start(&callbacks, NULL, options);

    mg_set_request_handler(ctx, "/json", json_output, NULL);
    mg_set_request_handler(ctx, "/", root_handler, NULL);

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
	// faup_terminate(fh);

	return 0;
}

