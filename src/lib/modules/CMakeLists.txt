set(MODULES_AVAILABLE
emulation_ie.lua
uppercase.lua
)

install(FILES ${MODULES_AVAILABLE} DESTINATION share/faup/modules_available)

install(CODE "file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/faup/modules_enabled)")
