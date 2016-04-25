#include <stdio.h>

#include <faup/faup.h>
#include <faup/decode.h>
#include <faup/output.h>
#include <faup/parson.h>

int main(int argc, char **argv)
{
  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;
  int i;

  faup_handler_t *fh;
  faup_options_t *options;

  char *url_origin;
  
  options = faup_options_new();
  if (!options) {
    fprintf(stderr, "Error: cannot allocate faup options!\n");
  }
  options->output = FAUP_OUTPUT_JSON;
  fh = faup_init(options);
  
  root_value = json_parse_file(argv[1]);
  if (json_value_get_type(root_value) != JSONArray) {
    printf("Invalid test file!\n");
    return -1;
  }

  json_array = json_value_get_array(root_value);
  for (i = 0; i < json_array_get_count(json_array); i++) {
    json_object = json_array_get_object(json_array, i);

    url_origin = json_object_get_string(json_object, "url_origin");
    faup_decode(fh, url_origin, strlen(url_origin));
    // requires an allocated buffer
    /* char *faup_output_json_buffer_allocate(); */
    /* void faup_output_json_buffer(faup_handler_t const* fh, faup_options_t *opts, char *buffer) */

    /* printf("url origin:%s\n", json_object_get_string(json_object, "url_origin")); */
  }

  json_value_free(root_value);
  faup_terminate(fh);

  return 0;
}
