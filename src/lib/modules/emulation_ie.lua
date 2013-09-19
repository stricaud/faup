-- Internet Explorer Emulation module
-- Written by Sebastien Tricaud
--
-- For internet explorer, the following URIs are all equivalent:
--  http://www.example.com/sample/
--  http://www.EXAMPLE.com/sample/#
--  Http://www.ex%41mple.com/s%61mple/
--  http://www.example.com/sample/arbitrary/.%2e/

function hextochar(hex)
	local num = tonumber(hex, 16)
	return string.char(num)
end

function faup_url_in(url)
	-- Http://www.ex%41mple.com/s%61mple/
	new_url = string.gsub(url, "%%([0-9a-fA-F][0-9a-fA-F])", hextochar)
 
 	-- http://www.EXAMPLE.com/sample/#
	last_char = string.sub(new_url, -1, -1)
	if last_char == "#" then
		new_url = string.sub(new_url, 0, -2)
	end

	--  http://www.example.com/sample/arbitrary/.%2e/
	new_url = string.gsub(new_url, "(/%w+/%.%.)", "")
	new_url_len = string.len(new_url)
	last_url_len = 0
	while (new_url_len ~= last_url_len) do
		last_url_len = new_url_len
		new_url = string.gsub(new_url, "(/%w+/%.%.)", "")
		new_url_len = string.len(new_url)
	end

	return string.lower(new_url)
end
