#!/usr/bin/python
import sys
import re
import os
import envoy
import shutil

FAUP_TEST_CMD="./src/tests/test_faup.sh"
URLS_FILE="../src/tests/urls.txt"
URLS_ORIGIN="../src/tests/urls.txt.ref"
URLS_CMP="./src/tests/urls.txt.cmp"

failed_test_re = re.compile("\s+(?P<id>\d+) \- (?P<testname>\S+) \(Failed\)")

def handle_one_test(testname):
    print(str(failed_test.group('testname')))
    try:
        os.makedirs("./_test-identify/%s" % (testname))
    except:
        pass
    envoy.run("%s %s" % (FAUP_TEST_CMD, testname))
    shutil.copyfile(URLS_ORIGIN, "./_test-identify/%s/%s" % (testname, "urls.txt.ref"))
    shutil.copyfile(URLS_CMP, "./_test-identify/%s/%s" % (testname, "urls.txt.cmp"))
    shutil.copyfile(URLS_FILE, "./_test-identify/%s/%s" % (testname, "urls.txt"))

for line in sys.stdin.readlines():
    failed_test = failed_test_re.match(line)
    if failed_test:
        handle_one_test(failed_test.group('testname'))


