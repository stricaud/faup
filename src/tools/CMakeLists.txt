include_directories(AFTER ${FAUP_INCLUDE_DIRS})

set(FAUP_SRC_FILES
faup.c
shell.c
)

if(WIN32)
set(WIN32_PORTABLE
win32-getopt.c
)
else()
set(WIN32_PORTABLE)
endif(WIN32)

add_executable(faup ${FAUP_SRC_FILES} ${WIN32_PORTABLE})
target_link_libraries(faup ${FAUP_LIBRARY})

install(TARGETS faup DESTINATION bin COMPONENT Application)
