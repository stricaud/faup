include_directories(AFTER ${FAUP_INCLUDE_DIRS})

set(FAUP_SRC_FILES
faup.c
shell.c
)

if (${LIBGTCACA_FOUND})
  set(FAUP_SRC_FILES
    ${FAUP_SRC_FILES}
    browse.c
    )
endif()


if(WIN32)
set(WIN32_PORTABLE
win32-getopt.c
)
else()
set(WIN32_PORTABLE)
endif(WIN32)


set(FAUP_LINK_LIBRARIES
	${FAUP_LIBRARY}
)

if(${LUA_FOUND})
	include_directories(BEFORE ${LUA_INCLUDE_DIRS})

	set(FAUP_LINK_LIBRARIES
		${FAUP_LINK_LIBRARIES}
		${LUA_LDFLAGS}
	)
endif()
if(${LIBGTCACA_FOUND})

	include_directories(BEFORE ${LUA_INCLUDE_DIRS})

	set(FAUP_LINK_LIBRARIES
		${FAUP_LINK_LIBRARIES}
		${LIBCACA_LDFLAGS}
		${LIBGTCACA_LDFLAGS}
	)

endif()

add_executable(faup ${FAUP_SRC_FILES} ${WIN32_PORTABLE})
target_link_libraries(faup ${FAUP_LINK_LIBRARIES} ${CMAKE_DL_LIBS})

install(TARGETS faup DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT Application)
