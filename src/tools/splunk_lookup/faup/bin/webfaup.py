#!/usr/bin/env python

import os
import csv
import sys
import json
import base64
import urllib
import platform
import logging, logging.handlers

##########
# CONFIG # 
##########

FAUP_SERVER = "127.0.0.1"
FAUP_SERVER_PORT = "9876"

header  = [
	'url', 
	'url_scheme', 'url_credential', 'url_subdomain', 'url_domain', 'url_domain_without_tld', 
	'url_host', 'url_tld', 'url_port', 'url_resource_path', 'url_query_string', 'url_fragment'
]

#############
# FUNCTIONS #
#############

def setup_logger():
        """                       
        Setup a logger for our lookup
        """

        logger = logging.getLogger('faup')
        logger.setLevel(logging.DEBUG)

        file_handler = logging.handlers.RotatingFileHandler(os.environ['SPLUNK_HOME'] + '/var/log/splunk/faup.log' )
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        file_handler.setFormatter(formatter)

        logger.addHandler(file_handler)

        return logger


def run_faup(logger, url_value):
	"""
	Submit the provided URL to FAUP web service and return a
	JSON dict.
	"""
	url_base64 = base64.b64encode(url_value)
	args = urllib.urlencode({'url': url_base64})
   
	try:
		query = "http://%s:%s/json?%s" % (FAUP_SERVER, FAUP_SERVER_PORT, args)
		f = urllib.urlopen(query)
		#logger.debug("query=%s" % query)
		faup_json = f.read()
	except Exception, e:
		logger.error("Error running the faup query (%s): %s" % (query, str(e)))
		return {}

	try:
		json_tree = json.loads(faup_json)
		json_modified = {}
		for (k, v) in json_tree.iteritems():
			json_modified["url_" + k] = v
		return json_modified
	except Exception, e:
		logger.error("Error loading Json : %s" % str(e))

	return {}
# eof run_faup()

########
# MAIN #
########
if( len(sys.argv) != 2 ):
	print "Usage: python faup.py url"
	sys.exit(0)

logger = setup_logger()

csv_in  = csv.DictReader(sys.stdin) # automatically use the first line as header
csv_out = csv.DictWriter(sys.stdout, header)

# write header
csv_out.writerow(dict(zip(header,header)))

for row in csv_in:
	res = run_faup(logger, row['url'])
	row.update(res)
	csv_out.writerow(row)

