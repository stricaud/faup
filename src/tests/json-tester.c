#include <stdio.h>
#include <faup/parson.h>

int main(int argc, char **argv)
{

  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;
  int i;
  
  root_value = json_parse_file(argv[1]);
  if (json_value_get_type(root_value) != JSONArray) {
    printf("Invalid test file!\n");
    return -1;
  }

  json_array = json_value_get_array(root_value);
  for (i = 0; i < json_array_get_count(json_array); i++) {
    json_object = json_array_get_object(json_array, i);
    printf("url origin:%s\n", json_object_get_string(json_object, "url_origin"));
  }

  json_value_free(root_value);
  
  return 0;
}
