#!/bin/bash
#
# DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
# Version 2, December 2004 
#
# Copyright (C) 2012 Adrien Guinet <adrien@guinet.me>
# Copyright (C) 2013-2016 Sebastien Tricaud <sebastien@honeynet.org>
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
FAUP_MODULES_DIR="${faup-project_SOURCE_DIR}/src/lib/modules"
FAUP_TOOL="${faup-project_BINARY_DIR}/src/tools/faup"

URLS="$SRC_TEST_DIR/urls.txt"
URLS_CMP="$BIN_TEST_DIR/urls.txt.cmp"

. $SRC_TEST_DIR/ticktick.sh


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

function test_module
{
    MODULE_NAME=$1

    FAUP_OPTIONS=$(cat $SRC_TEST_DIR/modules/$1.options)
    REF_FILE="$SRC_TEST_DIR/modules/$1.ref"
    URLS_FILE="$SRC_TEST_DIR/modules/$1"

    CMD="$FAUP_TOOL -m $MODULE_NAME $FAUP_OPTIONS"

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
    echo "Where test_type can be one of them: issue"
    exit 42
fi

case $1 in
    TLD_One_Only) test_tld_one_only;;
    JSON) test_json;;
    JSON_TLD_One_Only) test_json_tld_one_only;;
    issue) test_issue $2;;
    Url_Argument) test_argument "http://foo:bar@www3.altavista.digital.com:8080/index.php1?tada=da&fremo=genial#anchor1234" "$SRC_TEST_DIR/ref-files/argument.txt";;
    File_Argument) test_argument $URLS "$SRC_TEST_DIR/someurls.txt";;
    module) test_module $2;;
    *) echo "Unknown option '$1'"
    exit 42
    ;;
esac

