#include <stdio.h>
#include <stdlib.h>

#include <faup/faup.h>
#include <faup/features.h>
#include <faup/decode.h>
#include <faup/output.h>
#include <faup/parson.h>

int compare_features(JSON_Object *json_object, faup_feature_t feature, const char *url_origin, char *json_obj_str)
{
  char *json_feature;
  char *faup_feature;  
  int ret;
  
  /* if (!faup_features_exist(feature)) { return -1; } */
  
  json_feature = json_object_get_string(json_object, json_obj_str);

  if (!faup_features_exist(feature)) {
    if (json_feature[0] == '\0') {
      return 0;
    }
    return -1;
  }

  /* printf("size:%d, pos:%d\n", feature.size, feature.pos); */
  /* printf("url origin:%s\n", url_origin); */
  /* printf("at pos:%c\n", url_origin[feature.pos]); */
  faup_feature = malloc(feature.size + 1);
  memcpy((void *)faup_feature, (const void *)&url_origin[feature.pos], feature.size);
  faup_feature[feature.size] = '\0';

  ret = strcmp(json_feature, faup_feature);
  
  /* printf("feature:%s\n", faup_feature); */
  free(faup_feature);
  
  return ret;
}

int main(int argc, char **argv)
{
  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;
  int i;

  faup_handler_t *fh;
  faup_options_t *options;

  const char *url_origin;
  char *json_url_type;
  char *faup_url_type;

  int retval = 0;
  
  options = faup_options_new();
  if (!options) {
    fprintf(stderr, "Error: cannot allocate faup options!\n");
  }
  options->output = FAUP_OUTPUT_JSON;
  fh = faup_init(options);
  
  root_value = json_parse_file(argv[1]);
  if (!root_value) {
    printf("root_value is null, error parsing file %s\n", argv[1]);
    return -1;
  }
  if (json_value_get_type(root_value) != JSONArray) {
    printf("Invalid file '%s': root is not a json array!\n", argv[1]);
    return -1;
  }

  json_array = json_value_get_array(root_value);
  for (i = 0; i < json_array_get_count(json_array); i++) {
    json_object = json_array_get_object(json_array, i);

    url_origin = json_object_get_string(json_object, "url_origin");
    /* printf("url origin:%s\n", url_origin); */
    faup_decode(fh, url_origin, strlen(url_origin));

    printf("testing url: '%s'\n", url_origin);
    
    /* scheme */
    printf("\tscheme: ");
    if (compare_features(json_object, fh->faup.features.scheme, url_origin, "expected_scheme") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected scheme '%s'!\n", url_origin, json_object_get_string(json_object, "expected_scheme"));
      retval += 1;;
    }
    /* credential */
    printf("\tcredential: ");
    if (compare_features(json_object, fh->faup.features.credential, url_origin, "expected_credential") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected credential '%s'!\n", url_origin, json_object_get_string(json_object, "expected_credential"));
      retval += 1;;
    }
    /* subdomain */
    printf("\tsubdomain: ");
    if (compare_features(json_object, fh->faup.features.subdomain, url_origin, "expected_subdomain") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected subdomain '%s'!\n", url_origin, json_object_get_string(json_object, "expected_subdomain"));
      retval += 1;;
    }
    /* domain */
    printf("\tdomain: ");
    if (compare_features(json_object, fh->faup.features.domain, url_origin, "expected_domain") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected domain '%s'!\n", url_origin, json_object_get_string(json_object, "expected_domain"));
      retval += 1;;
    }
    /* domain without tld */
    printf("\tdomain without tld: ");
    if (compare_features(json_object, fh->faup.features.domain_without_tld, url_origin, "expected_domain_without_tld") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected domain_without_tld '%s'!\n", url_origin, json_object_get_string(json_object, "expected_domain_without_tld"));
      retval += 1;;
    }
    /* host */
    printf("\thost: ");
    if (compare_features(json_object, fh->faup.features.host, url_origin, "expected_host") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected host '%s'!\n", url_origin, json_object_get_string(json_object, "expected_host"));
      retval += 1;;
    }
    /* tld */
    printf("\ttld: ");
    if (compare_features(json_object, fh->faup.features.tld, url_origin, "expected_tld") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected tld '%s'!\n", url_origin, json_object_get_string(json_object, "expected_tld"));
      retval += 1;;
    }
    /* port */
    printf("\tport: ");
    if (compare_features(json_object, fh->faup.features.port, url_origin, "expected_port") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected port '%s'!\n", url_origin, json_object_get_string(json_object, "expected_port"));
      retval += 1;;
    }
    /* resource path */
    printf("\tresource path: ");
    if (compare_features(json_object, fh->faup.features.resource_path, url_origin, "expected_resource_path") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected resource_path '%s'!\n", url_origin, json_object_get_string(json_object, "expected_resource_path"));
      retval += 1;;
    }
    /* query string */
    printf("\tquery string: ");
    if (compare_features(json_object, fh->faup.features.query_string, url_origin, "expected_query_string") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected query_string '%s'!\n", url_origin, json_object_get_string(json_object, "expected_query_string"));
      retval += 1;;
    }
    /* fragment */
    printf("\tfragment: ");
    if (compare_features(json_object, fh->faup.features.fragment, url_origin, "expected_fragment") == 0) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected fragment '%s'!\n", url_origin, json_object_get_string(json_object, "expected_fragment"));
      retval += 1;;
    }
    /* url type */
    printf("\turl type: ");
    faup_url_type = faup_output_get_string_from_url_type(fh);
    json_url_type = json_object_get_string(json_object, "expected_url_type");
    if (!strcmp(faup_url_type, json_url_type)) {
      printf("OK\n");
    } else {
      printf("Error: from '%s' expected url_type '%s' but got '%s'!\n", url_origin, json_url_type, faup_url_type);
      retval += 1;;
    }
  }

  json_value_free(root_value);
  faup_terminate(fh);

  return retval;
}
