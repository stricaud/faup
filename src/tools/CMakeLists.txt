include_directories(AFTER ${FAUP_INCLUDE_DIRS})

if(WIN32)
set(WIN32_PORTABLE
win32-getopt.c
)
else()
set(WIN32_PORTABLE)
endif(WIN32)

add_executable(faup faup.c ${WIN32_PORTABLE})
target_link_libraries(faup ${FAUP_LIBRARY})

install(TARGETS faup DESTINATION bin COMPONENT Application)
