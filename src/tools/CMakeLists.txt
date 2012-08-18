include_directories(AFTER ${FURL_INCLUDE_DIRS})

add_executable(furl furl.c)
target_link_libraries(furl ${FURL_LIBRARY})

install(TARGETS furl DESTINATION bin)
