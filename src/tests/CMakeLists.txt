enable_testing()

configure_file("test_faup.sh.cmake" "test_faup.sh")

set(TEST_FAUP_SCRIPT "${faup-project_BINARY_DIR}/src/tests/test_faup.sh")
set(URLS_JSON "${faup-project_SOURCE_DIR}/src/tests/urls.json")

add_test(Url_Argument      ${TEST_FAUP_SCRIPT} Url_Argument)
add_test(File_Argument     ${TEST_FAUP_SCRIPT} File_Argument)

# Cover all the issues that we had (if applicable)
add_test(Issue24           ${TEST_FAUP_SCRIPT} issue 24)
add_test(Issue27           ${TEST_FAUP_SCRIPT} issue 27)
add_test(Issue28           ${TEST_FAUP_SCRIPT} issue 28)
add_test(Issue30           ${TEST_FAUP_SCRIPT} issue 30)
add_test(Issue69           ${TEST_FAUP_SCRIPT} issue 69)
add_test(Issue125           ${TEST_FAUP_SCRIPT} issue 125)

# Some C tests
include_directories(AFTER ${FAUP_INCLUDE_DIRS})

if(LINUX)
	add_executable(multithreads multithreads.c)
	target_link_libraries(multithreads ${FAUP_LIBRARY} "pthread" ${CMAKE_DL_LIBS})
	add_test(MultiThreads ${TEST_FAUP_SCRIPT} multithreads)
endif(LINUX)

# C API/ABI issues
add_executable(issue36 issue36.c)
target_link_libraries(issue36 ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Issue36 issue36)

add_executable(issue120 issue120.c)
target_link_libraries(issue120 ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Issue120 issue120)

# Snapshot Tests
add_executable(snapshot snapshot.c)
target_link_libraries(snapshot ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Snapshot snapshot)

# Hash Table Tests
add_executable(hashtable hashtable.c)
target_link_libraries(hashtable ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Hashtable hashtable)

# JSON Urls test
#if("${CJSON_FOUND}")
# add_executable(json-tester json-tester.c)
# target_link_libraries(json-tester ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
# add_test(JsonURLsTester json-tester ${URLS_JSON})
#endif()

# URL Split test
add_executable(urlsplit urlsplit.c)
target_link_libraries(urlsplit ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Urlsplit urlsplit)
