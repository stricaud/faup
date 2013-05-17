include_directories(AFTER "include/" ${FAUP_INCLUDE_DIRS})

set(FAUP_LIB_SRC_FILES
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
include/faup/features.h
include/faup/faup.h
include/faup/decode.h
include/faup/options.h
include/faup/output.h
include/faup/portable.h
include/faup/tld.h
include/faup/tld-tree.h
)

set(FAUP_LINK_LIBRARIES
)

# Our static library
add_library(faup_static ${FAUP_LIB_SRC_FILES})
set_target_properties(faup_static PROPERTIES SOVERSION 1)
target_link_libraries(faup_static ${FAUP_LINK_LIBRARIES})

# Our dynamic library
add_library(faupl SHARED ${FAUP_LIB_SRC_FILES})
set_target_properties(faupl PROPERTIES SOVERSION 1)
target_link_libraries(faupl ${FAUP_LINK_LIBRARIES})


install(TARGETS faupl LIBRARY DESTINATION lib)

