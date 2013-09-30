--
-- Write all the URLs into the file ~/.faup_urls
-- so we can track anything we've done with faup
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

	 data_out = orig_url .. "\t" .. decoded_url['domain_without_tld'] .. "\t" .. decoded_url['tld'] .. "\n"

	 homedir = os.getenv("HOME")
	 -- package.config:sub(1,1) will return the path separator
	 faup_url_file = homedir .. package.config:sub(1,1) .. ".faup_urls.txt"

	 file = io.open(faup_url_file, "a")
	 file:write(data_out)
	 file:close()

	 return orig_url, positions_and_sizes
end

