enable_testing()

add_test(Vanilla ${faup-project_SOURCE_DIR}/src/tests/test_faup.sh vanilla)
add_test(TLD_One_Only ${faup-project_SOURCE_DIR}/src/tests/test_faup.sh tld_one_only)
add_test(JSON ${faup-project_SOURCE_DIR}/src/tests/test_faup.sh json)
add_test(JSON_TLD_One_Only ${faup-project_SOURCE_DIR}/src/tests/test_faup.sh json_tld_one_only)
