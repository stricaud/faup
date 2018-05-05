set(MODULES_AVAILABLE
emulation_ie.lua
ipv4toint.lua
printcsv.lua
redis-url-threatintel.lua
writeall.lua
writeinput.lua
uppercase.lua
)

install(FILES ${MODULES_AVAILABLE} DESTINATION share/faup/modules_available)

install(DIRECTORY DESTINATION share/faup/modules_enabled)
