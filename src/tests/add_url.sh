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

if [ $# -ne 1 ]; then
	echo "Usage: $0 url" 1>&2
	echo "This will add 'url' in the list of URL to test." 1>&2
	exit 1
fi

# Get script absolute directory
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
URLFILE="$SCRIPTDIR/urls.txt"

echo $1 >>"$URLFILE"
if [ $? -ne 0 ]; then
	echo "Error while adding URL to urls.txt... Exiting."
	exit 1
fi

FAUP_OUT=$("$SCRIPTDIR/../tools/faup" "$1")
if [ $? -ne 0 ]; then
	echo "Error while adding URL to urls.txt... Exiting."
	head -n-1 "$URLFILE" >"$URLFILE.tmp"
	mv "$URLFILE.tmp" "$URLFILE"
	exit 1
fi

echo $FAUP_OUT >>"$SCRIPTDIR/urls.txt.ref"

echo "URL added to faup test case."
