include_directories(AFTER "include/")

set(FAUP_LIB_SRC_FILES
decode.c
features.c
faup.c
portable.c
)

set(FAUP_LIB_HDR_FILES
include/faup/features.h
include/faup/faup.h
include/faup/decode.h
include/faup/portable.h
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

