--
-- Sample module that replaces the CSV writer
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

function faup_output(orig_url, positions_and_sizes)

	scheme = extract_string(orig_url, "scheme", positions_and_sizes)
	credential = extract_string(orig_url, "credential", positions_and_sizes)
	subdomain = extract_string(orig_url, "subdomain", positions_and_sizes)
	domain = extract_string(orig_url, "domain", positions_and_sizes)
	domain_without_tld = extract_string(orig_url, "domain_without_tld", positions_and_sizes)
	host = extract_string(orig_url, "host", positions_and_sizes)
	tld = extract_string(orig_url, "tld", positions_and_sizes)
	port = extract_string(orig_url, "port", positions_and_sizes)
	resource_path = extract_string(orig_url, "resource_path", positions_and_sizes)
	query_string = extract_string(orig_url, "query_string", positions_and_sizes)
	fragment = extract_string(orig_url, "fragment", positions_and_sizes)

	io.write(scheme,",", credential,",", subdomain,",", domain,",", domain_without_tld,",", host,",", tld,",", port,",", resource_path,",", query_string,",", fragment,"\n")

	return orig_url, positions_and_sizes
end
