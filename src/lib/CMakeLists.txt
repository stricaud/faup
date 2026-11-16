include_directories(AFTER "include/" ${FAUP_INCLUDE_DIRS})


if(WIN32)
	# asprintf/vasprintf, inet_pton and memrchr are missing from the Windows C runtimes;
	# snprintf/vsnprintf exist in every toolchain we support, so bsd-snprintf.c is not built.
	set(WIN32_PORTABLE
	compat/bsd-asprintf.c
	compat/inet_pton.c
	compat/memrchr.c
	)
	# Snapshot files need POSIX directory APIs (dirent, nftw): not available on Windows yet.
	# The URL parsing library itself is complete.
	set(POSIX_ONLY_SRC)
else()
	set(WIN32_PORTABLE)
	set(POSIX_ONLY_SRC
	snapshot-file.c
	utils.c
	)
endif(WIN32)

if(WEBSERVER)
	set(WEBSERVER_SRC
	civetweb/md5.inl
	civetweb/civetweb.c
	webserver.c
	)
else()
	set(WEBSERVER_SRC)
endif(WEBSERVER)


if(${LUA_FOUND})

set(LUA_MODULES_SRC_FILES
modules.c
)
set(LUA_MODULES_HDR_FILES
include/faup/modules.h
)

include_directories(AFTER ${LUA_INCLUDE_DIRS})

set(FAUP_LINK_LIBRARIES
${FAUP_LINK_LIBRARIES}
${LUA_LDFLAGS}
)

endif()

set(MINIZ_SOURCE miniz/miniz.c miniz/miniz_zip.c miniz/miniz_tinfl.c miniz/miniz_tdef.c)
include_directories(AFTER "miniz/" ${FAUP_INCLUDE_DIRS})

set(FAUP_LIB_SRC_FILES
${WIN32_PORTABLE}
${WEBSERVER_SRC}
${LUA_MODULES_SRC_FILES}
${MINIZ_SOURCE}
${POSIX_ONLY_SRC}
b64/cencode.c
b64/cdecode.c
hash/hash.c
hash/htable.c
datadir.c
decode.c
features.c
faup.c
options.c
output.c
portable.c
snapshot.c
tld.c
tld-tree.c
)

set(FAUP_LIB_HDR_FILES
${LUA_MODULES_HDR_FILES}
include/faup/compat.h
include/faup/datadir.h
include/faup/decode.h
include/faup/errors.h
include/faup/faup.h
include/faup/features.h
include/faup/handler.h
include/faup/options.h
include/faup/output.h
include/faup/portable.h
include/faup/return-codes.h
include/faup/scheme-codes.h
include/faup/snapshot.h
include/faup/snapshot-file.h
include/faup/tld.h
include/faup/tld-tree.h
include/faup/urllengths.h
include/faup/utils.h
include/faup/webserver.h
)

# Our static library
add_library(faup_static ${FAUP_LIB_SRC_FILES})
set_target_properties(faup_static PROPERTIES SOVERSION 1)
target_link_libraries(faup_static ${FAUP_LINK_LIBRARIES})

# Our dynamic library
add_library(faupl SHARED ${FAUP_LIB_SRC_FILES})
set_target_properties(faupl PROPERTIES SOVERSION 1)
target_link_libraries(faupl ${FAUP_LINK_LIBRARIES})

# On Windows the DLL is a RUNTIME artifact: keep it next to the Unix libraries (lib/)
install(TARGETS faupl
	RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Libraries
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Libraries
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Libraries)
install(FILES ${FAUP_LIB_HDR_FILES} DESTINATION include/faup COMPONENT Headers)

add_subdirectory(modules)

