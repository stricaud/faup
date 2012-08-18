# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012

from ctypes import *
from bind import *

furl_get_version = bind("furl_get_version", c_char_p)
furl_init = bind("furl_init", c_void_p)
furl_decode = bind("furl_decode", None, c_void_p, c_char_p, c_uint)
furl_terminate = bind("furl_terminate", None, c_void_p)

furl_get_scheme_pos = bind("furl_get_scheme_pos", c_int, c_void_p)
furl_get_scheme_size = bind("furl_get_scheme_size", c_uint, c_void_p)
furl_get_credential_pos = bind("furl_get_credential_pos", c_int, c_void_p)
furl_get_credential_size = bind("furl_get_credential_size", c_uint, c_void_p)
furl_get_subdomain_pos = bind("furl_get_subdomain_pos", c_int, c_void_p)
furl_get_subdomain_size = bind("furl_get_subdomain_size", c_uint, c_void_p)
furl_get_domain_pos = bind("furl_get_domain_pos", c_int, c_void_p)
furl_get_domain_size = bind("furl_get_domain_size", c_uint, c_void_p)
furl_get_host_pos = bind("furl_get_host_pos", c_int, c_void_p)
furl_get_host_size = bind("furl_get_host_size", c_uint, c_void_p)
furl_get_tld_pos = bind("furl_get_tld_pos", c_int, c_void_p)
furl_get_tld_size = bind("furl_get_tld_size", c_uint, c_void_p)
furl_get_port_pos = bind("furl_get_port_pos", c_int, c_void_p)
furl_get_port_size = bind("furl_get_port_size", c_uint, c_void_p)
furl_get_resource_path_pos = bind("furl_get_resource_path_pos", c_int, c_void_p)
furl_get_resource_path_size = bind("furl_get_resource_path_size", c_uint, c_void_p)
furl_get_query_string_pos = bind("furl_get_query_string_pos", c_int, c_void_p)
furl_get_query_string_size = bind("furl_get_query_string_size", c_uint, c_void_p)
furl_get_fragment_pos = bind("furl_get_fragment_pos", c_int, c_void_p)
furl_get_fragment_size = bind("furl_get_fragment_size", c_uint, c_void_p)
