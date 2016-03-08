#!/bin/bash

wget http://s3.amazonaws.com/alexa-static/top-1m.csv.zip
unzip top-1m.csv.zip
cat top-1m.csv | cut -d, -f2 | ./src/tools/faup
