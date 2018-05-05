--
-- Following the idea from Jerome Kleinen in issue #88, adding the capability
-- to convert IPv4 addresses into an integer
--

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

	 if positions_and_sizes["url_type"] == "ipv4" then
	    -- from https://stackoverflow.com/questions/8200228/how-can-i-convert-an-ip-address-into-an-integer-with-lua
	    local o1,o2,o3,o4 = decoded_url['host']:match("(%d%d?%d?)%.(%d%d?%d?)%.(%d%d?%d?)%.(%d%d?%d?)" )
	    local num = 2^24*o1 + 2^16*o2 + 2^8*o3 + o4
	    print(math.floor(num))
	 end

	 return orig_url, positions_and_sizes
end

