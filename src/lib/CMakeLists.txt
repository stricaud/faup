include_directories(AFTER "include/")

set(FURL_LIB_SRC_FILES
decode.c
features.c
furl.c
)

set(FURL_LIB_HDR_FILES
include/furl/features.h
include/furl/furl.h
include/furl/decode.h
)

set(FURL_LINK_LIBRARIES
)

# Our static library
add_library(furl_static ${FURL_LIB_SRC_FILES})
set_target_properties(furl_static PROPERTIES SOVERSION 1)
target_link_libraries(furl_static ${FURL_LINK_LIBRARIES})

# Our dynamic library
add_library(furll SHARED ${FURL_LIB_SRC_FILES})
set_target_properties(furll PROPERTIES SOVERSION 1)
target_link_libraries(furll ${FURL_LINK_LIBRARIES})


install(TARGETS furll LIBRARY DESTINATION lib)

