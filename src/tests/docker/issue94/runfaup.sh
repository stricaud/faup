#!/bin/bash -x

echo "----Start"
echo ""
echo "-- 1 --"
echo "www.github.com" | /usr/local/bin/faup
echo $?
echo ""
echo "-- 2 --"
/usr/local/bin/faup  www.github.com
echo $?
echo ""
echo "----End"
