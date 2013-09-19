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
FAUP_DATA_DIR="${faup-project_SOURCE_DIR}/src/data"
FAUP_TOOL="${faup-project_BINARY_DIR}/src/tools/faup"

URLS="$SRC_TEST_DIR/urls.txt"
URLS_CMP="$BIN_TEST_DIR/urls.txt.cmp"

function test_generic
{
    FAUP_OPTS=$1
    URLS_REF=$2

    CMD="$FAUP_TOOL $FAUP_OPTS"

    # Execute faup on urls.txt and compare to the reference output
    $CMD < "$URLS" > "$URLS_CMP" ||exit $?
    diff -u "$URLS_CMP" "$URLS_REF" >/dev/null
    RET=$?
    if [ $RET -eq 0 ]; then rm "$URLS_CMP"; fi

    exit $RET
}

function test_argument
{
    ARGV1=$1
    URLS_REF=$2

    $FAUP_TOOL $ARGV1 > $URLS_CMP ||exit $?
    diff -u "$URLS_CMP" "$URLS_REF" >/dev/null
    RET=$?
    if [ $RET -eq 0 ]; then rm "$URLS_CMP"; fi

    exit $RET
}

function test_vanilla
{
    test_generic "" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla"
}

function test_vanilla_scheme
{
    test_generic "-f scheme" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.scheme"
}

function test_vanilla_credential
{
    test_generic "-f credential" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.credential"
}

function test_vanilla_subdomain
{
    test_generic "-f subdomain" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.subdomain"
}

function test_vanilla_domain
{
    test_generic "-f domain" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.domain"
}

function test_vanilla_host
{
    test_generic "-f host" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.host"
}

function test_vanilla_tld
{
    test_generic "-f tld" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.tld"
}

function test_vanilla_port
{
    test_generic "-f port" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.port"
}

function test_vanilla_resource_path
{
    test_generic "-f resource_path" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.resource_path"
}

function test_vanilla_query_string
{
    test_generic "-f query_string" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.query_string"
}

function test_vanilla_fragment
{
    test_generic "-f fragment" "$SRC_TEST_DIR/ref-files/urls.txt.vanilla.fragment"
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

function test_issue
{
    ISSUE_NB=$1

    FAUP_OPTIONS=$(cat $SRC_TEST_DIR/issues/$1.options)
    REF_FILE="$SRC_TEST_DIR/issues/$1.ref"
    URLS_FILE="$SRC_TEST_DIR/issues/$1"

    CMD="$FAUP_TOOL $FAUP_OPTIONS"

    # Execute faup on issues/ticket-id and compare to the reference output
    $CMD < "$URLS_FILE" > "$URLS_CMP" ||exit $1
    diff -u "$URLS_CMP" "$REF_FILE" >/dev/null
    RET=$?
    if [ $RET -eq 0 ]; then rm "$URLS_CMP"; fi

    exit $RET
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
    vanilla_scheme) test_vanilla_scheme;;
    vanilla_credential) test_vanilla_credential;;
    vanilla_subdomain) test_vanilla_subdomain;;
    vanilla_domain) test_vanilla_domain;;
    vanilla_host) test_vanilla_host;;
    vanilla_tld) test_vanilla_tld;;
    vanilla_port) test_vanilla_port;;
    vanilla_resource_path) test_vanilla_resource_path;;
    vanilla_query_string) test_vanilla_query_string;;
    vanilla_fragment) test_vanilla_fragment;;
    tld_one_only) test_tld_one_only;;
    json) test_json;;
    json_tld_one_only) test_json_tld_one_only;;
    issue) test_issue $2;;
    url_arg) test_argument "http://foo:bar@www3.altavista.digital.com:8080/index.php1?tada=da&fremo=genial#anchor1234" "$SRC_TEST_DIR/ref-files/argument.txt";;
    file_arg) test_argument $URLS "$SRC_TEST_DIR/ref-files/urls.txt.vanilla";;
    *) echo "Unknown option '$1'"
    exit 42
    ;;
esac

