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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _faup_modules_t faup_modules_t;

struct _faup_modules_t {
	lua_State *lua_state;
};

faup_modules_t *faup_modules_new(void);
void faup_modules_terminate(faup_modules_t *modules);
const char *faup_modules_exec_url_in(faup_modules_t *modules, char *module, const char *url);


#ifdef __cplusplus
}
#endif

#endif	/* _FAUP_MODULES_H_ */