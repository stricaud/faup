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
add_library(furll ${FURL_LIB_SRC_FILES})
set_target_properties(furll PROPERTIES SOVERSION 1)
target_link_libraries(furll ${FURL_LINK_LIBRARIES})

# Our dynamic library
add_library(furls SHARED ${FURL_LIB_SRC_FILES})
set_target_properties(furls PROPERTIES SOVERSION 1)
target_link_libraries(furls ${FURL_LINK_LIBRARIES})


#INSTALL(TARGETS furl LIBRARY DESTINATION .
#		     RUNTIME DESTINATION .)

