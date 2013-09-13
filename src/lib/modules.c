/*
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * Version 2, December 2004 
 *
 * Copyright (C) 2012-2013 Sebastien Tricaud <sebastien@honeynet.org> 
 *
 * Everyone is permitted to copy and distribute verbatim or modified 
 * copies of this license document, and changing it is allowed as long 
 * as the name is changed. 
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <stdlib.h>

#include <faup/modules.h>

faup_modules_t *faup_modules_new(void)
{
	faup_modules_t *modules;

	modules = malloc(sizeof(faup_modules_t));

	modules->lua_state = luaL_newstate();

	luaL_openlibs(modules->lua_state);

	return modules;
}

void faup_modules_terminate(faup_modules_t *modules)
{
	lua_close(modules->lua_state);
	free(modules);
}

// Fixme, we should avoid loading the module everytime we load the function
// It should be done at initialization time. I haven't figured how to handle
// lua states to work with multiple file like we do. There should be a way.
// Maybe I should simply have one lua_state per module.
const char *faup_modules_exec_url_in(faup_modules_t *modules, char *module, const char *url)
{
	int retval;

	retval = luaL_loadfile(modules->lua_state, module);
	if (retval) goto err;

	// Lua library requires that we load the file first
	retval = lua_pcall(modules->lua_state, 0, 0, 0);
	if (retval) goto err;

	lua_getglobal(modules->lua_state, "faup_url_in");
	lua_pushstring(modules->lua_state, url);

	retval = lua_pcall(modules->lua_state, 1, 1, 0);
	if (retval) goto err;

	return lua_tostring(modules->lua_state, -1);

err:
	fprintf(stderr, "*** Error: %s\n", lua_tostring(modules->lua_state, -1));
	return NULL;
}

int faup_modules_exec_extracted_fields(int module_id, lua_State *lua_state)
{

  return 0;
}


