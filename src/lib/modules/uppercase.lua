-- Sample module that changes the given url as uppercase 

function faup_url_in(url)
	 local new_url = string.upper(url)
	 return new_url
end

-- function faup_output(orig_url, positions_and_sizes)
--
--	local new_positions_and_sizes = {numfields=1}
--
--	for k,v in pairs(positions_and_sizes) do
--		--io.write("k:",k, " v:", v, "\n")
--		new_positions_and_sizes.numfields = new_positions_and_sizes.numfields + 1
--		new_positions_and_sizes[tostring(k)] = v
--	end

--	return orig_url, new_positions_and_sizes
--end
