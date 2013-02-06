include_directories(AFTER ${FAUP_INCLUDE_DIRS})

add_executable(faup faup.c)
target_link_libraries(faup ${FAUP_LIBRARY})

install(TARGETS faup DESTINATION bin)
