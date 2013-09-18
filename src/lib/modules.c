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

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <faup/faup.h>
#include <faup/datadir.h>
#include <faup/modules.h>

int faup_modules_new(faup_handler_t *fh)
{
	int count;

	switch(fh->options->exec_modules) {
		case FAUP_MODULES_NOEXEC:
			fh->modules = NULL;
		break;
		case FAUP_MODULES_EXECPATH:
			fh->modules = faup_modules_load_from_datadir();
//			if (fh->modules) {
//				new_url = faup_modules_exec_url_in_by_module_name(fh->modules, "sample_uppercase.lua", url);
//			}
		break;
		case FAUP_MODULES_EXECARG:
			fh->modules = faup_modules_load_from_arg(fh->options->modules_argv, fh->options->modules_argc);
		break;
		default:
			fprintf(stderr, "*** Huh? We should never be there (%s)!\n", __FUNCTION__);
	}

	return 0;
}

void faup_modules_terminate(faup_modules_t *modules)
{
	if (!modules) {
		return;
	}

	int	count = 0;
	while (count < modules->nb_modules) {
		if (modules->module[count].lua_state) {
			lua_close(modules->module[count].lua_state);
		}
		free(modules->module[count].module_path);
		free(modules->module[count].module_name);
		count++;
	}
	free(modules->module);
	free(modules);
}

faup_modules_t *faup_modules_load_from_datadir(void)
{
	faup_modules_t *modules = NULL;
	int count = 0;

	modules = malloc(sizeof(faup_modules_t));
	modules->nb_modules = faup_modules_foreach_filelist(NULL, NULL, NULL);
	modules->module = malloc(sizeof(faup_module_t) * modules->nb_modules);
	//memset(modules, 0, sizeof(faup_module_t) * modules->nb_modules);
	faup_modules_foreach_filelist(modules, faup_module_register, (int *)count);

	return modules;
}

faup_modules_t *faup_modules_load_from_arg(char **argv, int argc)
{
	int count;

	for (count = 0; count < argc; count++) {
		printf("arg:%s\n", argv[count]);
	}

	return NULL;
}


void faup_module_register(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
	int retval;

	retval = asprintf(&modules->module[count].module_path, "%s%s%s", modules_dir, FAUP_OS_DIRSEP, module);
	if (retval < 0) {
		fprintf(stderr, "Cannot allocate in %s with parameter '%s%s%s'\n", __FUNCTION__, modules_dir, FAUP_OS_DIRSEP, module);
		return;
	}

	modules->module[count].module_name = strdup(module);

//#ifdef FAUP_DEBUG
//	printf("Loading module from path:%s\n", modules->module[count].module_path);
//#endif

	modules->module[count].lua_state = luaL_newstate();
	luaL_openlibs(modules->module[count].lua_state);

	retval = luaL_loadfile(modules->module[count].lua_state, modules->module[count].module_path);
	if (retval) goto err;

	// Lua library requires that we load the file first
	retval = lua_pcall(modules->module[count].lua_state, 0, 0, 0);
	if (retval) goto err;

	return;

err:
	fprintf(stderr, "*** Error: %s\n", lua_tostring(modules->module[count].lua_state, -1));
}


int faup_modules_foreach_filelist(faup_modules_t *modules, void (*cb_modules_foreach)(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count), void *user_data)
{
	char *modules_dir;
	DIR  *modules_dir_fp;
	struct dirent *modules_dir_file;
	int count = 0;

	modules_dir = faup_datadir_get_file("modules_enabled");
	modules_dir_fp = opendir(modules_dir);
	if (!modules_dir_fp) {
		return -1;
	}
	modules_dir_file = readdir(modules_dir_fp);
	while (modules_dir_file) {
		if (modules_dir_file->d_name[0] != '.') {
			size_t filelen = strlen(modules_dir_file->d_name);
			if (filelen > FAUP_MODULE_NAME_MAXLEN) {
				fprintf(stderr, "*** Error: Module file name too long (>128). Won't execute!\n");
				count++;
				continue;
			}

			if (cb_modules_foreach) cb_modules_foreach(modules, modules_dir, modules_dir_file->d_name, user_data, count);
			count++;
		}
		modules_dir_file = readdir(modules_dir_fp);
	}
	closedir(modules_dir_fp);

	return count;
}


void faup_modules_list(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
	printf("%s\n", module);
}


const char *faup_modules_exec_url_in_by_module_name(faup_modules_t *modules, char *module, const char *url)
{
	int retval;
	int	count = 0;

	if (!modules) {
		fprintf(stderr, "%s was given no modules struct!\n", __FUNCTION__);
		return NULL;
	}

	while (count < modules->nb_modules) {
		if (!strcmp(module, modules->module[count].module_name)) {
			lua_getglobal(modules->module[count].lua_state, "faup_url_in");
			lua_pushstring(modules->module[count].lua_state, url);

			retval = lua_pcall(modules->module[count].lua_state, 1, 1, 0);
			if (retval) goto err;

			return lua_tostring(modules->module[count].lua_state, -1);
		}

		count++;
	}

	// We have not executed any module
	return NULL;

err:
	fprintf(stderr, "*** Error: %s\n", lua_tostring(modules->module[count].lua_state, -1));
	return NULL;
}

int faup_modules_exec_extracted_fields(int module_id, lua_State *lua_state)
{

  return 0;
}

faup_modules_transformed_url_t *faup_modules_decode_url_start(faup_handler_t const* fh, const char *url, size_t url_len)
{
	faup_modules_t *modules = (faup_modules_t *)fh->modules;
	faup_modules_transformed_url_t *transformed_url = NULL;
	const char *new_url = NULL;
	int count;
	int retval;

	// No modules have been loaded, no need to go further!
	if (!modules) {
		return NULL;
	}

	transformed_url = malloc(sizeof(faup_modules_transformed_url_t));
	if (!transformed_url) {
		fprintf(stderr, "Cannot allocate URL for transformed url by modules!\n");
		return NULL;
	}

	// For every loaded module, we run the function 'faup_url_in'
	for (count=0; count < modules->nb_modules; count++) {
			lua_getglobal(modules->module[count].lua_state, "faup_url_in");
			if (!new_url) {
				lua_pushstring(modules->module[count].lua_state, url);
			} else {
				lua_pushstring(modules->module[count].lua_state, new_url);				
			}
			retval = lua_pcall(modules->module[count].lua_state, 1, 1, 0);
			if (retval) {
				fprintf(stderr, "*** Error: %s\n", lua_tostring(modules->module[count].lua_state, -1));
				return NULL;
			}

			new_url = lua_tostring(modules->module[count].lua_state, -1);	
	}

	if (new_url) {
		transformed_url->url = new_url;
		transformed_url->url_len = strlen(new_url);
	} else {
		transformed_url->url = url;
		transformed_url->url_len = url_len;
	}

	return transformed_url;
}

void faup_modules_transformed_url_free(faup_modules_transformed_url_t *transformed_url)
{
	//free((void *)transformed_url->url);
	free(transformed_url);
}

void _faup_add_keyval_dict(faup_modules_t *modules, int count, char *key, int val)
{
	lua_pushstring(modules->module[count].lua_state, key); // -2
	lua_pushinteger(modules->module[count].lua_state, val); // -3
	lua_settable(modules->module[count].lua_state, -3);

}

bool faup_modules_url_output(faup_handler_t *fh, FILE* out)
{
	int retval;
	int count;
	faup_modules_t *modules = fh->modules;
	const char *transformed_url = NULL;

	bool module_executed = false;

	if (!modules) {
		return false;
	}
	
	for (count = 0; count < modules->nb_modules; count++) {
		lua_getglobal(modules->module[count].lua_state, "faup_output");

		lua_pushstring(modules->module[count].lua_state, fh->faup.org_str);

		// Table for positions
		lua_newtable(modules->module[count].lua_state); // -1
		_faup_add_keyval_dict(modules, count, "scheme.pos", fh->faup.features.scheme.pos);
		_faup_add_keyval_dict(modules, count, "scheme.size", fh->faup.features.scheme.size);
		_faup_add_keyval_dict(modules, count, "hierarchical.pos", fh->faup.features.hierarchical.pos);
		_faup_add_keyval_dict(modules, count, "hierarchical.size", fh->faup.features.hierarchical.size);
		_faup_add_keyval_dict(modules, count, "credential.pos", fh->faup.features.credential.pos);
		_faup_add_keyval_dict(modules, count, "credential.size", fh->faup.features.credential.size);
		_faup_add_keyval_dict(modules, count, "host.pos", fh->faup.features.host.pos);
		_faup_add_keyval_dict(modules, count, "host.size", fh->faup.features.host.size);
		_faup_add_keyval_dict(modules, count, "subdomain.pos", fh->faup.features.subdomain.pos);
		_faup_add_keyval_dict(modules, count, "subdomain.size", fh->faup.features.subdomain.size);
		_faup_add_keyval_dict(modules, count, "domain.pos", fh->faup.features.domain.pos);
		_faup_add_keyval_dict(modules, count, "domain.size", fh->faup.features.domain.size);
		_faup_add_keyval_dict(modules, count, "domain_without_tld.pos", fh->faup.features.domain_without_tld.pos);
		_faup_add_keyval_dict(modules, count, "domain_without_tld.size", fh->faup.features.domain_without_tld.size);
		_faup_add_keyval_dict(modules, count, "tld.pos", fh->faup.features.tld.pos);
		_faup_add_keyval_dict(modules, count, "tld.size", fh->faup.features.tld.size);
		_faup_add_keyval_dict(modules, count, "port.pos", fh->faup.features.port.pos);
		_faup_add_keyval_dict(modules, count, "port.size", fh->faup.features.port.size);
		_faup_add_keyval_dict(modules, count, "resource_path.pos", fh->faup.features.resource_path.pos);
		_faup_add_keyval_dict(modules, count, "resource_path.size", fh->faup.features.resource_path.size);
		_faup_add_keyval_dict(modules, count, "query_string.pos", fh->faup.features.query_string.pos);
		_faup_add_keyval_dict(modules, count, "query_string.size", fh->faup.features.query_string.size);
		_faup_add_keyval_dict(modules, count, "fragment.pos", fh->faup.features.fragment.pos);
		_faup_add_keyval_dict(modules, count, "fragment.size", fh->faup.features.fragment.size);

		// If the function does not exists, we just ignore it
		retval = lua_pcall(modules->module[count].lua_state, 2, 2, 0);
		if (retval == 0) {
			module_executed = true;
			transformed_url = lua_tostring(modules->module[count].lua_state, -1);
			if (transformed_url) {
				fh->faup.org_str = transformed_url;
			}

    		lua_pushnil(modules->module[count].lua_state);
    		const char *k;
    		int v;
   			while (lua_next(modules->module[count].lua_state, -2)) { // The table is is -2 in the stack
        		v = lua_tointeger(modules->module[count].lua_state, -1);
        		lua_pop(modules->module[count].lua_state, 1);
        		k = lua_tostring(modules->module[count].lua_state, -1);

        		if (!strcmp(k, "scheme.pos")) {
        			fh->faup.features.scheme.pos = v;
        		}
        		if (!strcmp(k, "scheme.size")) {
        			fh->faup.features.scheme.size = v;

        		}
        		if (!strcmp(k, "hierarchical.pos")) {
        			fh->faup.features.hierarchical.pos = v;
        		}
        		if (!strcmp(k, "hierarchical.size")) {
        			fh->faup.features.hierarchical.size = v;

        		}
        		if (!strcmp(k, "credential.pos")) {
        			fh->faup.features.credential.pos = v;
        		}
        		if (!strcmp(k, "credential.size")) {
        			fh->faup.features.credential.size = v;

        		}
        		if (!strcmp(k, "host.pos")) {
        			fh->faup.features.host.pos = v;
        		}
        		if (!strcmp(k, "host.size")) {
        			fh->faup.features.host.size = v;

        		}
        		if (!strcmp(k, "subdomain.pos")) {
        			fh->faup.features.subdomain.pos = v;
        		}
        		if (!strcmp(k, "subdomain.size")) {
        			fh->faup.features.subdomain.size = v;

        		}
        		if (!strcmp(k, "domain.pos")) {
        			fh->faup.features.domain.pos = v;
        		}
        		if (!strcmp(k, "domain.size")) {
        			fh->faup.features.domain.size = v;

        		}
        		if (!strcmp(k, "domain_without_tld.pos")) {
        			fh->faup.features.domain_without_tld.pos = v;
        		}
        		if (!strcmp(k, "domain_without_tld.size")) {
        			fh->faup.features.domain_without_tld.size = v;

        		}
        		if (!strcmp(k, "tld.pos")) {
        			fh->faup.features.tld.pos = v;
        		}
        		if (!strcmp(k, "tld.size")) {
        			fh->faup.features.tld.size = v;

        		}
        		if (!strcmp(k, "port.pos")) {
        			fh->faup.features.port.pos = v;
        		}
        		if (!strcmp(k, "port.size")) {
        			fh->faup.features.port.size = v;

        		}
        		if (!strcmp(k, "resource_path.pos")) {
        			fh->faup.features.resource_path.pos = v;
        		}
        		if (!strcmp(k, "resource_path.size")) {
        			fh->faup.features.resource_path.size = v;

        		}
        		if (!strcmp(k, "query_string.pos")) {
        			fh->faup.features.query_string.pos = v;
        		}
        		if (!strcmp(k, "query_string.size")) {
        			fh->faup.features.query_string.size = v;

        		}
        		if (!strcmp(k, "fragment.pos")) {
        			fh->faup.features.fragment.pos = v;
        		}
        		if (!strcmp(k, "fragment.size")) {
        			fh->faup.features.fragment.size = v;

        		}
   			}


		}
	}

	return module_executed;
}

