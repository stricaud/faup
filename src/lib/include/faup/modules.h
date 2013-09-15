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

#ifndef _FAUP_MODULES_H_
#define _FAUP_MODULES_H_

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define FAUP_MODULE_NAME_MAXLEN 128

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _faup_modules_t faup_modules_t;
typedef struct _faup_module_t faup_module_t;

struct _faup_module_t {
	char *module_path;
	char *module_name;
	lua_State *lua_state;
};

struct _faup_modules_t {
	lua_State *lua_state;
	int nb_modules;
	faup_module_t *module;
};

faup_modules_t *faup_modules_new(void);
void faup_modules_terminate(faup_modules_t *modules);
void _faup_module_register(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count);
const char *faup_modules_exec_url_in_by_module_name(faup_modules_t *modules, char *module, const char *url);

// Returns the number of modules
int faup_modules_foreach_filelist(faup_modules_t *modules, void (*cb_modules_foreach)(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count), void *user_data);
void faup_modules_list(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count);


#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_MODULES_H_ */