enable_testing()

configure_file("test_faup.sh.cmake" "test_faup.sh")

set(TEST_FAUP_SCRIPT "${faup-project_BINARY_DIR}/src/tests/test_faup.sh")

add_test(Vanilla               ${TEST_FAUP_SCRIPT} vanilla)
add_test(VanillaScheme         ${TEST_FAUP_SCRIPT} vanilla_scheme)
add_test(VanillaCredential     ${TEST_FAUP_SCRIPT} vanilla_credential)
add_test(VanillaSubdomain      ${TEST_FAUP_SCRIPT} vanilla_subdomain)
add_test(VanillaDomain         ${TEST_FAUP_SCRIPT} vanilla_domain)
add_test(VanillaHost           ${TEST_FAUP_SCRIPT} vanilla_host)
add_test(VanillaTLD            ${TEST_FAUP_SCRIPT} vanilla_tld)
add_test(VanillaPort           ${TEST_FAUP_SCRIPT} vanilla_port)
add_test(VanillaResourcePath   ${TEST_FAUP_SCRIPT} vanilla_resource_path)
add_test(VanillaQueryString    ${TEST_FAUP_SCRIPT} vanilla_query_string)
add_test(VanillaFragment       ${TEST_FAUP_SCRIPT} vanilla_fragment)

add_test(TLD_One_Only      ${TEST_FAUP_SCRIPT} tld_one_only)
add_test(JSON              ${TEST_FAUP_SCRIPT} json)
add_test(JSON_TLD_One_Only ${TEST_FAUP_SCRIPT} json_tld_one_only)
add_test(Url_Argument      ${TEST_FAUP_SCRIPT} url_arg)
add_test(File_Argument     ${TEST_FAUP_SCRIPT} file_arg)

# Cover all the issues that we had (if applicable)
add_test(Issue24           ${TEST_FAUP_SCRIPT} issue 24)
add_test(Issue27           ${TEST_FAUP_SCRIPT} issue 27)
add_test(Issue28           ${TEST_FAUP_SCRIPT} issue 28)
add_test(Issue30           ${TEST_FAUP_SCRIPT} issue 30)


# Some C tests
include_directories(AFTER ${FAUP_INCLUDE_DIRS})

if(LINUX)
	add_executable(multithreads multithreads.c)
	target_link_libraries(multithreads ${FAUP_LIBRARY} "pthread" )
	add_test(MultiThreads ${TEST_FAUP_SCRIPT} multithreads)
endif(LINUX)

add_executable(base64 base64.c)
target_link_libraries(base64 ${FAUP_LIBRARY})
add_test(Base64 ${TEST_FAUP_SCRIPT} base64)
