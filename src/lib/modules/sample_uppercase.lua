-- Sample module that changes the given url as uppercase 

function faup_url_in(url)
	 local new_url = string.upper(url)
	 return new_url
end

function faup_extracted_fields(url, url_fields)
	 local tab_out = {numfields=1}
	 for k,v in pairs(tab_in) do
	     tab_out.numfields = tab_out.numfields + 1
	     tab_out[tostring(k)] = string.upper(tostring(v))
	 end
         tab_out.numfields = tostring(tab_out.numfields)

	 return tab_out
end
