#!/bin/bash

FAUP=../../build/src/tools/faup

mkdir ./ref-files

# Vanilla
cat urls.txt |$FAUP  > ./ref-files/urls.txt.vanilla

# TLD One Only
cat urls.txt |$FAUP -t  > ./ref-files/urls.txt.tld_one_only

# JSON
cat urls.txt |$FAUP -o json  > ./ref-files/urls.txt.json

# JSON TLD One Only
cat urls.txt |$FAUP -o json -t  > ./ref-files/urls.txt.json_tld_one_only

# Argument + File Argument
$FAUP "http://foo:bar@www3.altavista.digital.com:8080/index.php1?tada=da&fremo=genial#anchor1234" > ./ref-files/argument.txt

# IE Emulation
$FAUP -e ie urls.txt > ./ref-files/urls.txt.emulation_ie

