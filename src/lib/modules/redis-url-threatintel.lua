--
-- Use redis to store a counter of the URLs that we see
-- You need the lua-redis binding from: https://github.com/nrk/redis-lua
--

local redis = require 'redis'
local client = redis.connect('127.0.0.1', 6379)

function extract_string(orig_url, feature, positions_and_sizes)
	feature_pos=feature .. ".pos"
	feature_size=feature .. ".size"

	if positions_and_sizes[feature_pos] >= 0 
	then 
   	     feature_str = string.sub(orig_url, positions_and_sizes[feature_pos] + 1, positions_and_sizes[feature_pos] + positions_and_sizes[feature_size]) 
	else 
	     feature_str = "" 
	end
	 
	return feature_str
end

function get_strings_from_positions_and_sizes(orig_url, positions_and_sizes)

	table_strings = {}

	table_strings['scheme']             = extract_string(orig_url, "scheme", positions_and_sizes)
	table_strings['credential']         = extract_string(orig_url, "credential", positions_and_sizes)
	table_strings['subdomain']          = extract_string(orig_url, "subdomain", positions_and_sizes)
	table_strings['domain']             = extract_string(orig_url, "domain", positions_and_sizes)
	table_strings['domain_without_tld'] = extract_string(orig_url, "domain_without_tld", positions_and_sizes)
	table_strings['host']               = extract_string(orig_url, "host", positions_and_sizes)
	table_strings['tld']                = extract_string(orig_url, "tld", positions_and_sizes)
	table_strings['port']               = extract_string(orig_url, "port", positions_and_sizes)
	table_strings['resource_path']      = extract_string(orig_url, "resource_path", positions_and_sizes)
	table_strings['query_string']       = extract_string(orig_url, "query_string", positions_and_sizes)
	table_strings['fragment']           = extract_string(orig_url, "fragment", positions_and_sizes)

	return table_strings
end

function faup_output(orig_url, positions_and_sizes)
	 decoded_url=get_strings_from_positions_and_sizes(orig_url, positions_and_sizes)

	 client:incr('scheme::' .. decoded_url['scheme'])
	 client:incr('credential::' .. decoded_url['credential'])
	 client:incr('subdomain::' .. decoded_url['subdomain'])
	 client:incr('domain::' .. decoded_url['domain'])
	 client:incr('domain_without_tld::' .. decoded_url['domain_without_tld'])
	 client:incr('host::' .. decoded_url['host'])
	 client:incr('tld::' .. decoded_url['tld'])
	 client:incr('port::' .. decoded_url['port'])
	 client:incr('resource_path::' .. decoded_url['resource_path'])
	 client:incr('query_string::' .. decoded_url['query_string'])
	 client:incr('fragment::' .. decoded_url['fragment'])

	 return orig_url, positions_and_sizes
end

