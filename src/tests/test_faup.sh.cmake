#!/bin/bash
#
# DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
# Version 2, December 2004 
#
# Copyright (C) 2012 Adrien Guinet <adrien@guinet.me>
# Copyright (C) 2013 Sebastien Tricaud <sebastien@honeynet.org>
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

SRC_TEST_DIR="${faup-project_SOURCE_DIR}/src/tests"
BIN_TEST_DIR="${faup-project_BINARY_DIR}/src/tests"
FAUP_TOOL="${faup-project_BINARY_DIR}/src/tools/faup"

URLS="$SRC_TEST_DIR/urls.txt"
URLS_CMP="$BIN_TEST_DIR/urls.txt.cmp"

function test_generic
{
    FAUP_OPTS=$1
    URLS_REF=$2

    CMD="$FAUP_TOOL $FAUP_OPTS"

    # Execute faup on urls.txt and compare to the reference output
    $CMD < "$URLS" > "$URLS_CMP" ||exit $1
    diff -u "$URLS_CMP" "$URLS_REF" >/dev/null
    RET=$?
    if [ $RET -eq 0 ]; then rm "$URLS_CMP"; fi

    exit $RET
}

function test_vanilla
{
    test_generic "" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla"
}

function test_tld_one_only
{
    test_generic "-t" "$SRC_TEST_DIR/ref-files/urls.txt.tld_one_only"
}

function test_json
{
    test_generic "-o json" "$SRC_TEST_DIR/ref-files/urls.txt.json"
}


function test_json_tld_one_only
{
    test_generic "-o json -t" "$SRC_TEST_DIR/ref-files/urls.txt.json_tld_one_only"
}


#
# Main
#

if [ $# -lt 1 ]
then
    echo "$0 test_type"
    echo "Where test_type can be one of them: vanilla, tld_one_only, json, json_tld_one_only"
    exit 42
fi

case $1 in
    vanilla) test_vanilla;;
    tld_one_only) test_tld_one_only;;
    json) test_json;;
    json_tld_one_only) test_json_tld_one_only;;
    *) echo "Unknown option '$1'"
    exit 42
    ;;
esac

