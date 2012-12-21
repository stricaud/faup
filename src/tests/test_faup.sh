#!/bin/bash
#
# DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
# Version 2, December 2004 
#
# Copyright (C) 2012 Adrien Guinet <adrien@guinet.me>
#
# Everyone is permitted to copy and distribute verbatim or modified 
# copies of this license document, and changing it is allowed as long 
# as the name is changed. 
#
# DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
# TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 
#
#  0. You just DO WHAT THE FUCK YOU WANT TO.

# Test faup output
# faup output format is :
# scheme,credential,domain,tld,port,resource_path,query_string,fragment
# 
# To add an URL to the list of URL to test, simply use the provided add_url.sh script :
# $ ./add_url.sh http://www.test.com/myweirdurl
# It will basically add http://www.test.com/myweirdurl to urls.txt, and the output of faup to urls.txt.ref

# Get script absolute directory
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
URLS="$SCRIPTDIR/urls.txt"
URLS_CMP="$SCRIPTDIR/urls.txt.cmp"
URLS_REF="$SCRIPTDIR/urls.txt.ref"

# Execute faup on urls.txt and compare to the reference output
"$SCRIPTDIR/../tools/faup" <"$URLS" >"$URLS_CMP" ||exit $1
diff -u "$URLS_CMP" "$URLS_REF" >/dev/null
RET=$?
if [ $RET -eq 0 ]; then rm "$URLS_CMP"; fi

exit $RET
