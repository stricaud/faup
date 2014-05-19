--
-- Write all the URLs given to faup into the file ~/.faup_urls.input
-- so we can track anything we've done with faup
--

function faup_url_in(url)
	 homedir = os.getenv("HOME")
	 -- package.config:sub(1,1) will return the path separator
	 faup_url_file = homedir .. package.config:sub(1,1) .. ".faup_urls.input"

	 utc = os.time(now)
	 date_time = os.date("[%Y-%m-%d %H:%M:%S]", utc)

	 data_out = date_time .. " " .. os.clock() .. " " .. url .. "\n"

	 file = io.open(faup_url_file, "a")
	 file:write(data_out)
	 file:close()
end


