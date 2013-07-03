#!/bin/bash

FAUP=../../build/src/tools/faup

mkdir ./ref-files

# Vanilla
cat urls.txt |$FAUP  > ./ref-files/urls.txt.vanilla

#
# With test every single field, one by one
#

# Vanilla scheme
cat urls.txt |$FAUP -f scheme > ./ref-files/urls.txt.vanilla.scheme

# Vanilla credential
cat urls.txt |$FAUP -f credential > ./ref-files/urls.txt.vanilla.credential

# Vanilla subdomain
cat urls.txt |$FAUP -f subdomain > ./ref-files/urls.txt.vanilla.subdomain

# Vanilla domain
cat urls.txt |$FAUP -f domain > ./ref-files/urls.txt.vanilla.domain

# Vanilla host
cat urls.txt |$FAUP -f host > ./ref-files/urls.txt.vanilla.host

# Vanilla tld
cat urls.txt |$FAUP -f tld > ./ref-files/urls.txt.vanilla.tld

# Vanilla port
cat urls.txt |$FAUP -f port > ./ref-files/urls.txt.vanilla.port

# Vanilla resource_path
cat urls.txt |$FAUP -f resource_path > ./ref-files/urls.txt.vanilla.resource_path

# Vanilla query_string
cat urls.txt |$FAUP -f query_string  > ./ref-files/urls.txt.vanilla.query_string

# Vanilla fragment
cat urls.txt |$FAUP -f fragment  > ./ref-files/urls.txt.vanilla.fragment

#
# End of Vanilla tests
#

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

