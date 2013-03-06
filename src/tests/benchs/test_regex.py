#!/usr/bin/python
import sys
import re

regex_1 = "^(http|https|ftp)\://([a-zA-Z0-9\.\-]+(\:[a-zA-Z0-9\.&amp;%\$\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|localhost|([a-zA-Z0-9\-]+\.)*[a-zA-Z0-9\-]+\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(\:[0-9]+)*(/($|[a-zA-Z0-9\.\,\?\'\\\+&amp;%\$#\=~_\-]+))*$"

def test_regex(urls_file):
    success = 0
    failures = 0
    pcre = re.compile(regex_1)
    for line in urls_file.readlines():
        m = pcre.match(line)
        if m:
            success += 1
#            print(m.group(4))
        else:
#            print("failure with '%s'" % (line))
            failures += 1

    print("Success:%d;Failures:%d" % (success, failures))


if __name__ == "__main__":
    urls_file = open(sys.argv[1], "r")
    test_regex(urls_file)
    urls_file.close()
