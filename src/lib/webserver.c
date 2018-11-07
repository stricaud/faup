#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <sys/mman.h>
#include <sys/fcntl.h> // For O_RDWR
#include <stdlib.h>
#include <pthread.h>
#define __USE_XOPEN_EXTENDED // for strdup() BS glibc now needs
#include <string.h>

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/b64/cdecode.h>
#include <faup/output.h>
#include <faup/snapshot.h>
#include <faup/snapshot-file.h>
#include <faup/utlist.h>

#include <faup/webserver.h>


static struct mg_context *ctx;
static faup_handler_t *_fh;
static faup_options_t *_faup_opts;
pthread_mutex_t lock;

struct snapshots_list_t {
  faup_snapshot_t *snapshot;
  char *name;
  struct snapshots_list_t *next;
};
struct snapshots_list_t *open_snapshots = NULL;


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
                Take an action: <br/> \
		url: No URL given. Please use the uri such as /json?url=base64_of_your_url </br> \
                snapshot:\n \
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
	    goto out;
    }
    int ret = mg_get_var(ri->query_string, strlen(ri->query_string), "url", url, FAUP_MAXLEN);
    if (ret < 0) {
	    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	    mg_printf(conn, "No URL given. Please use the uri such as /json?url=base64_of_your_url\n");
	    goto out;
    }

    url_len = strlen(url);
    if (url_len < 1) {
    	fprintf(stderr, "ERROR: Invalid given URL of size:%zd\n", url_len);
	goto out;
    }

    base64_init_decodestate(&s);
    url_unbase64 = (char*)malloc(FAUP_MAX_JSON_BUFFER_SIZE);

    url_outlen = base64_decode_block(url, url_len, url_unbase64, &s);

    faup_decode(_fh, (const char *)url_unbase64, url_outlen);

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

    faup_output_json_buffer(_fh, _faup_opts, buffer);

    mg_printf(conn, "%s", (char *)buffer);

    free(url_unbase64);

 out:    
    pthread_mutex_unlock(&lock);

    return 1;
}

int snapshot_action_create_handler(char *snapshot_name, struct mg_connection *conn, const struct mg_request_info *ri)
{
  struct snapshots_list_t *el;

  el = malloc(sizeof(struct snapshots_list_t));
  if (!el) {
    fprintf(stderr, "could not allocate the element!\n");
    return -1;
  }

  el->snapshot = faup_snapshot_open(snapshot_name);

  if (el->snapshot) {
    el->name = strdup(snapshot_name);
    LL_APPEND(open_snapshots, el);
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    mg_printf(conn, "Snapshot '%s' created with success\n", snapshot_name);
  } else {
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    mg_printf(conn, "Error creating snapshot '%s'\n", snapshot_name);
  }
  
  return 0;
}

int snapshot_action_open_handler(char *snapshot_name, struct mg_connection *conn, const struct mg_request_info *ri)
{
  struct snapshots_list_t *el = NULL;
  #define AS_MAXLEN 1024
  char as[AS_MAXLEN];
  int open_as = 0;
  int ret;
  
  el = malloc(sizeof(struct snapshots_list_t));
  if (!el) {
    fprintf(stderr, "could not allocate the element!\n");
    return -1;
  }

  mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

  ret = mg_get_var(ri->query_string, strlen(ri->query_string), "as", as, AS_MAXLEN);
  if (ret < 0) {
    open_as = 1;
  }

  
  el->snapshot = faup_snapshot_read(snapshot_name);
  free(el->snapshot->name);
  if (open_as) {
    el->snapshot->name = strdup(as);
  }
  if (el->snapshot) {
    if (open_as) {
      el->name = strdup(as);
    } else {
      el->name = strdup(snapshot_name);
    }
    LL_APPEND(open_snapshots, el);
    mg_printf(conn, "Snapshot %s opened with success\n", snapshot_name);
  } else {
    mg_printf(conn, "Error, could not open snapshot %s\n", snapshot_name);
  }
  
  return 0;
}

int snapshot_action_append_handler(char *snapshot_name, struct mg_connection *conn, const struct mg_request_info *ri)
{
  struct snapshots_list_t *el = NULL;
  int exists = 0;
  #define ITEM_MAXLEN 1024
  char item[ITEM_MAXLEN];
  #define KEY_MAXLEN 4096
  char key[KEY_MAXLEN];
  char key_unbase64[KEY_MAXLEN];
  base64_decodestate s;
  size_t key_outlen;
  
  int ret;
  
  mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

  ret = mg_get_var(ri->query_string, strlen(ri->query_string), "item", item, ITEM_MAXLEN);
  if (ret < 0) {
    mg_printf(conn, "Error, key 'item' not found\n");
    return -1;
  }
  ret = mg_get_var(ri->query_string, strlen(ri->query_string), "key", key, KEY_MAXLEN);
  if (ret < 0) {
    mg_printf(conn, "Error, key 'key' not found\n");
    return -1;
  }
  
  base64_init_decodestate(&s);
  key_outlen = base64_decode_block(key, strlen(key), key_unbase64, &s);  

  CDL_FOREACH(open_snapshots, el) {
    if (!strcmp(el->name, snapshot_name)) {
      exists = 1;
      faup_snapshot_append(el->snapshot, item, key_unbase64);
      mg_printf(conn, "Item %s appended with success\n", item);
    }
  }

  return exists ? 0 : -1;
}

int snapshot_action_get_handler(char *snapshot_name, struct mg_connection *conn, const struct mg_request_info *ri)
{
  struct snapshots_list_t *el = NULL;
  int exists = 0;
  #define ITEM_MAXLEN 1024
  char item[ITEM_MAXLEN];
  #define KEY_MAXLEN 4096
  char key[KEY_MAXLEN];
  char key_unbase64[KEY_MAXLEN];
  base64_decodestate s;
  size_t key_outlen;

  int ret;

  faup_snapshot_item_t *snapshot_item;
  faup_snapshot_value_count_t *vc = NULL;

  
  ret = mg_get_var(ri->query_string, strlen(ri->query_string), "item", item, ITEM_MAXLEN);
  if (ret < 0) {
    return -1;
  }
  ret = mg_get_var(ri->query_string, strlen(ri->query_string), "key", key, KEY_MAXLEN);
  if (ret < 0) {
    return -1;
  }

  base64_init_decodestate(&s);
  key_outlen = base64_decode_block(key, strlen(key), key_unbase64, &s);  
  
  CDL_FOREACH(open_snapshots, el) {
    if (!strcmp(el->name, snapshot_name)) {
      exists = 1;
      snapshot_item = faup_snapshot_item_get(el->snapshot, item);
      if (snapshot_item) {
	vc = faup_snapshot_value_count_get(snapshot_item, key_unbase64);
      }
    }
  }

  if (exists) {
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    if (vc) {
      mg_printf(conn, "%ld\n", vc->count);
    } else {
      mg_printf(conn, "0\n");
    }
  }
  
  return exists ? 0 : -1;
}

int snapshot_action_close_handler(char *snapshot_name, struct mg_connection *conn, const struct mg_request_info *ri)
{
  struct snapshots_list_t *el = NULL;
  int exists = 0;
  int ret;

  mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
  
  CDL_FOREACH(open_snapshots, el) {
    if (!strcmp(el->name, snapshot_name)) {
      exists = 1;
      
      ret = faup_snapshot_write(el->snapshot, ".");
      if (ret >= 0) {
	faup_snapshot_free(el->snapshot);
	LL_DELETE(open_snapshots, el);
	mg_printf(conn, "Snapshot '%s' closed with success\n", snapshot_name);
      } else {
	mg_printf(conn, "Error closing snapshot '%s'\n", snapshot_name);
      }
    }
  }


  
  return exists ? 0 : -1;
}


int snapshot_handler(struct mg_connection *conn, void *cbdata)
{
    const struct mg_request_info *ri = mg_get_request_info(conn);
    int ret;
    #define ACTION_MAXLEN 16
    char action[ACTION_MAXLEN];
    #define SNAPSHOTNAME_MAXLEN 512
    char snapshot[SNAPSHOTNAME_MAXLEN];
    
    pthread_mutex_lock(&lock);

    if (!ri->query_string) {
      mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
      mg_printf(conn, "Snapshot mode for the webserver can be used to:\n");
      mg_printf(conn, "<ul>\n");
      mg_printf(conn, "<li>create a new snapshot: /snapshot?action=create&snapshot=snapshotname</li>\n");
      mg_printf(conn, "<li>open an exists snapshot: /snapshot?action=open&snapshot=snapshotname&as=newname</li>\n");
      mg_printf(conn, "<li>add data to snapshot: /snapshot?action=append&snapshot=snapshotname&item=name&key=key</li>\n");
      mg_printf(conn, "<li>get data from snapshot: /snapshot?action=get&snapshot=snapshotname&item=name&key=key</li>\n");
      mg_printf(conn, "<li>close a snapshot: /snapshot?action=close&snapshot=snapshotname</li>\n");
      mg_printf(conn, "</ul>\n");
      goto out;
    }

    ret = mg_get_var(ri->query_string, strlen(ri->query_string), "action", action, ACTION_MAXLEN);
    if (ret < 0) {
	    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	    mg_printf(conn, "No action specified. Choose one amongst create, open, append, get, close\n");
	    goto out;
    }
    ret = mg_get_var(ri->query_string, strlen(ri->query_string), "snapshot", snapshot, SNAPSHOTNAME_MAXLEN);
    if (ret < 0) {
	    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	    mg_printf(conn, "No snapshot name given.\n");
	    goto out;
    }

    if (!strcmp(action, "create")) {
      snapshot_action_create_handler(snapshot, conn, ri);
    } else if (!strcmp(action, "open")) {
      snapshot_action_open_handler(snapshot, conn, ri);
    } else if (!strcmp(action, "append")) {
      ret = snapshot_action_append_handler(snapshot, conn, ri);
      if (ret < 0) {
	mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	mg_printf(conn, "Snapshot not found. Please create or open one.\n");
	goto out;
      }
    } else if (!strcmp(action, "get")) {
      snapshot_action_get_handler(snapshot, conn, ri);
    } else if (!strcmp(action, "close")) {
      snapshot_action_close_handler(snapshot, conn, ri);
    } else {
      mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
      mg_printf(conn, "Unknown action specified. Choose one amongst create, open, append, get, close\n");
      goto out;
    }


 out:
    pthread_mutex_unlock(&lock);

    return 1;
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
	if (!ctx) {
	  fprintf(stderr, "Error: cannot start webserver!\n");
	  return 1;
	}
	
	mg_set_request_handler(ctx, "/json", json_output, buffer);
	mg_set_request_handler(ctx, "/snapshot", snapshot_handler, NULL);
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

	return 0;
}

