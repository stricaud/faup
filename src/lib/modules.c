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

#ifdef LINUX
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>

#include <faup/faup.h>
#include <faup/datadir.h>
#include <faup/modules.h>

int faup_modules_new(faup_handler_t *fh)
{
	int count;

	if (!fh->options) {
		fprintf(stderr, "Options have not been initialized! Make sure they are set before calling %s\n", __FUNCTION__);
		return -1;
	}

	switch(fh->options->exec_modules) {
		case FAUP_MODULES_NOEXEC:
			fh->modules = NULL;
		break;
		case FAUP_MODULES_EXECPATH:
			fh->modules = faup_modules_load_from_datadir();

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
		if (modules->module[count].module_path) {
		  free(modules->module[count].module_path);
		}
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
	if (!modules) {
		fprintf(stderr, "Cannot allocate modules!\n");
		return NULL;
	}
	modules->nb_modules = faup_modules_foreach_filelist(NULL, NULL, NULL, NULL);
	if (modules->nb_modules <= 0) {
		// We have no modules enabled
		free(modules);
		return NULL;
	}
	modules->module = malloc(sizeof(faup_module_t) * modules->nb_modules);
	if (!modules->module) {
		fprintf(stderr, "Cannot allocate modules->module!\n");
		free(modules);
		return NULL;
	}
	faup_modules_foreach_filelist(modules, NULL, faup_module_register, (void *)(intptr_t)count);

	return modules;
}

faup_modules_t *faup_modules_load_from_arg(char **argv, int argc)
{
	faup_modules_t *modules = NULL;
	FILE *fp;
	int count;
	int modules_that_do_not_count = 0;
	int retval;


	bool a_module_was_registered;

	modules = malloc(sizeof(faup_modules_t));
	if (!modules) {
		fprintf(stderr, "Cannot allocate modules in %s\n", __FUNCTION__);
		return NULL;
	}
	modules->module = malloc(sizeof(faup_module_t) * argc);
	modules->nb_modules = 0;
	if (!modules->module) {
		fprintf(stderr, "Cannot allocate modules->module in %s\n", __FUNCTION__);
		free(modules);
		return NULL;
	}
	for (count = 0; count < argc; count++) {
		a_module_was_registered = false;
		// We try first to open a local file
		fp = fopen(argv[count], "r");
		if (fp) { // Found
			fclose(fp);

			retval = faup_module_register(modules, NULL, argv[count], NULL, count - modules_that_do_not_count);
			if (retval == 0) {
				modules->nb_modules++;
				a_module_was_registered = true;
			}
		} else {
			// The module was not discovered localy, so we get it from "modules_available"
			char *load_path;
			char *available_module;

			load_path = malloc(17 /* modules_available */ + 1 /* FAUP_OS_DIRSEP */ + strlen(argv[count]));
			retval = asprintf(&load_path, "modules_available%s%s", FAUP_OS_DIRSEP, argv[count]);
			available_module = faup_datadir_get_file(load_path);
			free(load_path);

			fp = fopen(available_module, "r");
			if (fp) {
				retval = faup_module_register(modules, NULL, available_module, NULL, count - modules_that_do_not_count);
				if (retval == 0) {
					modules->nb_modules++;
					a_module_was_registered = true;

				}
				fclose(fp);
			} else {
				fprintf(stderr, "Cannot register the module '%s': Not found in modules_available nor local path!\n", argv[count]);
			}

		}

		if (!a_module_was_registered) {
			modules_that_do_not_count++;
		}

	}

	return modules;
}


int faup_module_register(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
	int retval;

//	printf("Register module with module %s and count %d\n", module, count);

	if (modules_dir) {
		retval = asprintf(&modules->module[count].module_path, "%s%s%s", modules_dir, FAUP_OS_DIRSEP, module);
		if (retval < 0) {
			fprintf(stderr, "Cannot allocate in %s with parameter '%s%s%s'\n", __FUNCTION__, modules_dir, FAUP_OS_DIRSEP, module);
			return -1;
		}
	} else {
		modules->module[count].module_path = strdup(module);
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

	return 0;

err:
	fprintf(stderr, "*** Error(%s): %s\n", __FUNCTION__, lua_tostring(modules->module[count].lua_state, -1));
	free(modules->module[count].module_path);
	return -1;
}


int faup_modules_foreach_filelist(faup_modules_t *modules, char *force_path, int (*cb_modules_foreach)(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count), void *user_data)
{
	char *modules_dir;
	DIR  *modules_dir_fp;
	struct dirent *modules_dir_file;
	int count = 0;

	if (!force_path) {
		modules_dir = faup_datadir_get_file("modules_enabled");
	} else {
		modules_dir = force_path;
	}
	modules_dir_fp = opendir(modules_dir);
	if (!modules_dir_fp) {
		free(modules_dir);
		return -1;
	}

	modules_dir_file = readdir(modules_dir_fp);
	while (modules_dir_file) {
		if (modules_dir_file->d_name[0] != '.') {
			size_t filelen = strlen(modules_dir_file->d_name);
			if (filelen > FAUP_MODULE_NAME_MAXLEN) {
				fprintf(stderr, "*** Error(%s): Module file name too long (>128). Won't execute!\n", __FUNCTION__);
				count++;
				continue;
			}

			if (cb_modules_foreach) cb_modules_foreach(modules, modules_dir, modules_dir_file->d_name, user_data, count);
			count++;
		}
		modules_dir_file = readdir(modules_dir_fp);
	}
	closedir(modules_dir_fp);

	if (!force_path) {
		free(modules_dir);
	}

	return count;
}


void faup_modules_list(faup_modules_t *modules, char *modules_dir, char *module, void *user_data, int count)
{
	printf("%s\n", module);
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
		fprintf(stderr, "(%s) Cannot allocate URL for transformed url by modules!\n", __FUNCTION__);
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
				// This module has no faup_url_in function. Nothing wrong here, we can continue
				continue;
				//fprintf(stderr, "*** Error(%s): %s\n", __FUNCTION__, lua_tostring(modules->module[count].lua_state, -1));
				//return NULL;
			}

			new_url = lua_tostring(modules->module[count].lua_state, -1);	
	}

	if (new_url) {
		//printf("new url=%s, size=%zd\n", new_url, strlen(new_url));
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

void _faup_add_feature(faup_modules_t *modules, int count, faup_feature_t feature, char *pos_key, char *size_key)
{
		if (faup_features_exist(feature)) {
			_faup_add_keyval_dict(modules, count, pos_key, feature.pos);
			_faup_add_keyval_dict(modules, count, size_key, feature.size);
		} else {
			_faup_add_keyval_dict(modules, count, pos_key, -1);
			_faup_add_keyval_dict(modules, count, size_key, 0);			
		}
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
		if(!lua_isfunction(modules->module[count].lua_state, -1)) {
			//printf("No such function: 'faup_output'\n");
			// Since that function does not exists in the module, we skip it!
			continue;
		}

		lua_pushstring(modules->module[count].lua_state, fh->faup.org_str);

		// Table for positions
		lua_newtable(modules->module[count].lua_state); // -1

		_faup_add_feature(modules, count, fh->faup.features.scheme, "scheme.pos", "scheme.size");
		_faup_add_feature(modules, count, fh->faup.features.hierarchical, "hierarchical.pos", "hierarchical.size");
		_faup_add_feature(modules, count, fh->faup.features.credential, "credential.pos", "credential.size");
		_faup_add_feature(modules, count, fh->faup.features.host, "host.pos", "host.size");
		_faup_add_feature(modules, count, fh->faup.features.subdomain, "subdomain.pos", "subdomain.size");
		_faup_add_feature(modules, count, fh->faup.features.domain, "domain.pos", "domain.size");
		_faup_add_feature(modules, count, fh->faup.features.domain_without_tld, "domain_without_tld.pos", "domain_without_tld.size");
		_faup_add_feature(modules, count, fh->faup.features.tld, "tld.pos", "tld.size");
		_faup_add_feature(modules, count, fh->faup.features.port, "port.pos", "port.size");
		_faup_add_feature(modules, count, fh->faup.features.resource_path, "resource_path.pos", "resource_path.size");
		_faup_add_feature(modules, count, fh->faup.features.query_string, "query_string.pos", "query_string.size");
		_faup_add_feature(modules, count, fh->faup.features.fragment, "fragment.pos", "fragment.size");


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
		} else { // if (retval == 0) {
			fprintf(stderr, "*** Error(%s)[in script:%s]: %s\n", __FUNCTION__, modules->module[count].module_name, lua_tostring(modules->module[count].lua_state, -1));
		}
	}

	return module_executed;
}

