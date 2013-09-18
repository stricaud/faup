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
	new_url = string.gsub(url, "%%([0-9a-fA-F][0-9a-fA-F])", hextochar)
 
	return string.lower(new_url)
end
