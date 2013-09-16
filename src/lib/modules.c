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
#include <dirent.h>
#include <string.h>

#include <faup/faup.h>
#include <faup/datadir.h>
#include <faup/modules.h>

faup_modules_t *faup_modules_new(void)
{
	faup_modules_t *modules;
	intptr_t count = 0;

	modules = malloc(sizeof(faup_modules_t));
	modules->nb_modules = faup_modules_foreach_filelist(NULL, NULL, NULL);
	modules->module = malloc(sizeof(faup_module_t) * modules->nb_modules);

	faup_modules_foreach_filelist(modules, _faup_module_register, (intptr_t *)count);

	return modules;
}


void faup_modules_terminate(faup_modules_t *modules)
{
	int	count = 0;
	while (count < modules->nb_modules) {
		lua_close(modules->module[count].lua_state);
		free(modules->module[count].module_path);
		free(modules->module[count].module_name);
		count++;
	}
	free(modules->module);
	free(modules);
}


void _faup_module_register(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
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

faup_modules_transformed_url_t *faup_modules_decode_url_start(faup_options_t *options, const char *url, size_t url_len)
{
	faup_modules_transformed_url_t *transformed_url = NULL;
	faup_modules_t *modules;
	const char *new_url;

	transformed_url = malloc(sizeof(faup_modules_transformed_url_t));
	if (!transformed_url) {
		fprintf(stderr, "Cannot allocate URL for transformed url by modules!\n");
		return NULL;
	}

	modules = faup_modules_new();
	new_url = faup_modules_exec_url_in_by_module_name(modules, "sample_uppercase.lua", url);
	faup_modules_terminate(modules);

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
