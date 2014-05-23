# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012

from ctypes import *
from .bind import *

faup_get_version = bind("faup_get_version", c_char_p)
faup_init = bind("faup_init", c_void_p, c_void_p)
faup_decode = bind("faup_decode", c_char_p, c_void_p, c_char_p, c_size_t)
faup_terminate = bind("faup_terminate", None, c_void_p)

faup_get_scheme_pos = bind("faup_get_scheme_pos", c_int, c_void_p)
faup_get_scheme_size = bind("faup_get_scheme_size", c_uint, c_void_p)
faup_get_credential_pos = bind("faup_get_credential_pos", c_int, c_void_p)
faup_get_credential_size = bind("faup_get_credential_size", c_uint, c_void_p)
faup_get_subdomain_pos = bind("faup_get_subdomain_pos", c_int, c_void_p)
faup_get_subdomain_size = bind("faup_get_subdomain_size", c_uint, c_void_p)
faup_get_domain_pos = bind("faup_get_domain_pos", c_int, c_void_p)
faup_get_domain_size = bind("faup_get_domain_size", c_uint, c_void_p)
faup_get_domain_without_tld_pos = bind("faup_get_domain_without_tld_pos", c_int, c_void_p)
faup_get_domain_without_tld_size = bind("faup_get_domain_without_tld_size", c_uint, c_void_p)
faup_get_host_pos = bind("faup_get_host_pos", c_int, c_void_p)
faup_get_host_size = bind("faup_get_host_size", c_uint, c_void_p)
faup_get_tld_pos = bind("faup_get_tld_pos", c_int, c_void_p)
faup_get_tld_size = bind("faup_get_tld_size", c_uint, c_void_p)
faup_get_port_pos = bind("faup_get_port_pos", c_int, c_void_p)
faup_get_port_size = bind("faup_get_port_size", c_uint, c_void_p)
faup_get_resource_path_pos = bind("faup_get_resource_path_pos", c_int, c_void_p)
faup_get_resource_path_size = bind("faup_get_resource_path_size", c_uint, c_void_p)
faup_get_query_string_pos = bind("faup_get_query_string_pos", c_int, c_void_p)
faup_get_query_string_size = bind("faup_get_query_string_size", c_uint, c_void_p)
faup_get_fragment_pos = bind("faup_get_fragment_pos", c_int, c_void_p)
faup_get_fragment_size = bind("faup_get_fragment_size", c_uint, c_void_p)

faup_options_new = bind("faup_options_new", c_void_p)
faup_options_free = bind("faup_options_free", None, c_void_p)
