enable_testing()

configure_file("test_faup.sh.cmake" "test_faup.sh")

set(TEST_FAUP_SCRIPT "${faup-project_BINARY_DIR}/src/tests/test_faup.sh")

add_test(Vanilla               ${TEST_FAUP_SCRIPT} Vanilla)
add_test(VanillaScheme         ${TEST_FAUP_SCRIPT} VanillaScheme)
add_test(VanillaCredential     ${TEST_FAUP_SCRIPT} VanillaCredential)
add_test(VanillaSubdomain      ${TEST_FAUP_SCRIPT} VanillaSubdomain)
add_test(VanillaDomain         ${TEST_FAUP_SCRIPT} VanillaDomain)
add_test(VanillaHost           ${TEST_FAUP_SCRIPT} VanillaHost)
add_test(VanillaTLD            ${TEST_FAUP_SCRIPT} VanillaTLD)
add_test(VanillaPort           ${TEST_FAUP_SCRIPT} VanillaPort)
add_test(VanillaResourcePath   ${TEST_FAUP_SCRIPT} VanillaResourcePath)
add_test(VanillaQueryString    ${TEST_FAUP_SCRIPT} VanillaQueryString)
add_test(VanillaFragment       ${TEST_FAUP_SCRIPT} VanillaFragment)

add_test(TLD_One_Only      ${TEST_FAUP_SCRIPT} TLD_One_Only)
add_test(JSON              ${TEST_FAUP_SCRIPT} JSON)
add_test(JSON_TLD_One_Only ${TEST_FAUP_SCRIPT} JSON_TLD_One_Only)
add_test(Url_Argument      ${TEST_FAUP_SCRIPT} Url_Argument)
add_test(File_Argument     ${TEST_FAUP_SCRIPT} File_Argument)

# Cover all the issues that we had (if applicable)
add_test(Issue24           ${TEST_FAUP_SCRIPT} issue 24)
add_test(Issue27           ${TEST_FAUP_SCRIPT} issue 27)
add_test(Issue28           ${TEST_FAUP_SCRIPT} issue 28)
add_test(Issue30           ${TEST_FAUP_SCRIPT} issue 30)
add_test(Issue69           ${TEST_FAUP_SCRIPT} issue 69)

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

# Snapshot Tests
add_executable(snapshot snapshot.c)
target_link_libraries(snapshot ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Snapshot snapshot)

# Hash Table Tests
add_executable(hashtable hashtable.c)
target_link_libraries(hashtable ${FAUP_LIBRARY} ${CMAKE_DL_LIBS})
add_test(Hashtable hashtable)

