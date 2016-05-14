include_directories(AFTER ${FAUP_INCLUDE_DIRS})

set(FAUP_SRC_FILES
faup.c
shell.c
)

if (${LIBCACA_FOUND})
  set(FAUP_SRC_FILES
    ${FAUP_SRC_FILES}
    browse.c
    browse-widgets.c
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

if(${LUA51_FOUND})

	include_directories(AFTER ${LUA_INCLUDE_DIRS})

	set(FAUP_LINK_LIBRARIES
		${FAUP_LINK_LIBRARIES}
		${LUA_LIBRARIES}
	)
endif()
if(${LIBCACA_FOUND})

	include_directories(AFTER ${LUA_INCLUDE_DIRS})

	set(FAUP_LINK_LIBRARIES
		${FAUP_LINK_LIBRARIES}
		${LIBCACA_LIBRARIES}
	)

endif()

add_executable(faup ${FAUP_SRC_FILES} ${WIN32_PORTABLE})
target_link_libraries(faup ${FAUP_LINK_LIBRARIES} ${CMAKE_DL_LIBS})

install(TARGETS faup DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT Application)
