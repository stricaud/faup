enable_testing()

configure_file("test_faup.sh.cmake" "test_faup.sh")

set(TEST_FAUP_SCRIPT "${faup-project_BINARY_DIR}/src/tests/test_faup.sh")

add_test(Vanilla           ${TEST_FAUP_SCRIPT} vanilla)
add_test(TLD_One_Only      ${TEST_FAUP_SCRIPT} tld_one_only)
add_test(JSON              ${TEST_FAUP_SCRIPT} json)
add_test(JSON_TLD_One_Only ${TEST_FAUP_SCRIPT} json_tld_one_only)
add_test(Issue24 ${TEST_FAUP_SCRIPT} issue 24)
