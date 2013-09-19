include_directories(AFTER "include/" ${FAUP_INCLUDE_DIRS})


if(WIN32)
set(WIN32_PORTABLE
compat/bsd-asprintf.c
compat/bsd-snprintf.c
compat/inet_pton.c
compat/memrchr.c
)
else()
set(WIN32_PORTABLE)
endif(WIN32)


if(${LUA51_FOUND})

set(LUA_MODULES_SRC_FILES
modules.c
)
set(LUA_MODULES_HDR_FILES
include/faup/modules.h
)

include_directories(AFTER ${LUA_INCLUDE_DIR})

endif()

set(FAUP_LIB_SRC_FILES
${WIN32_PORTABLE}
${LUA_MODULES_SRC_FILES}
datadir.c
decode.c
features.c
faup.c
options.c
output.c
portable.c
tld.c
tld-tree.c
)

set(FAUP_LIB_HDR_FILES
include/faup/compat.h
include/faup/datadir.h
include/faup/decode.h
include/faup/features.h
include/faup/faup.h
${LUA_MODULES_HDR_FILES}
include/faup/options.h
include/faup/output.h
include/faup/portable.h
include/faup/tld.h
include/faup/tld-tree.h
)

set(FAUP_LINK_LIBRARIES
${LUA_LIBRARIES}
)

# Our static library
add_library(faup_static ${FAUP_LIB_SRC_FILES})
set_target_properties(faup_static PROPERTIES SOVERSION 1)
target_link_libraries(faup_static ${FAUP_LINK_LIBRARIES})

# Our dynamic library
add_library(faupl SHARED ${FAUP_LIB_SRC_FILES})
set_target_properties(faupl PROPERTIES SOVERSION 1)
target_link_libraries(faupl ${FAUP_LINK_LIBRARIES})

install(TARGETS faupl LIBRARY DESTINATION lib COMPONENT Libraries)

install(FILES ${FAUP_LIB_HDR_FILES} DESTINATION include/faup COMPONENT Headers)

add_subdirectory(modules)

