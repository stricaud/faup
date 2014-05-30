#!/usr/bin/env python

import csv
import sys
import os
import json
import logging, logging.handlers
import platform
#import subprocess
import envoy
import pprint, StringIO

def where_is_faup():
	if platform.system() == "Darwin":
		return os.environ['SPLUNK_HOME'] + "/etc/apps/faup/opt/faup-darwin"
	if platform.system() == "Linux":
		return os.environ['SPLUNK_HOME'] + "/etc/apps/faup/opt/faup-linux"

	# I don't know, so let's trust the system
	return "faup"

faup_bin = where_is_faup()

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
    url_value.replace("'", "''")
    url_value.replace('"', '""')
    url_value.decode('utf-8', 'ignore')

    # known bug:
    # urls with '\xAA' pattern in it.
    run_command = 'echo -n "%s" |%s -o json' % (url_value, faup_bin)

    try:
        faup_r = envoy.run(run_command)
        json_from_faup = faup_r.std_out
    except:
        logger.info("Error running the command (url=[%s]: %s" % (url_value, run_command))
        return None

    if not faup_r.std_err == "":
        logger.info("faup_r.std_err=%s" % (faup_r.std_err))

    try:
        json_tree = json.loads(json_from_faup)
        json_modified = {}
        for k, v in json_tree.iteritems():
            json_modified["url_" + k] = v

    #logger.info("json=%s" % json_modified)

        return json_modified
    except:
        logger.info("Error loading Json (url=[%s]): %s" % (url_value, json_from_faup))


    return None


# Faup output fields
# scheme credential subdomain domain domain_without_tld host tld port resource_path query_string fragment
def main():

    logger = setup_logger()

    if( len(sys.argv) != 2 ):
        print "Usage: python faup.py url"
        sys.exit(0)

    header  = [
        'url', 'url_scheme', 'url_credential', 'url_subdomain', 'url_domain',
        'url_domain_without_tld', 'url_host', 'url_tld', 'url_port', 'url_resource_path',
        'url_query_string', 'url_fragment'
    ]

    csv_in  = csv.DictReader(sys.stdin) # automatically use the first line as header
    csv_out = csv.DictWriter(sys.stdout, header)

    # write header
    csv_out.writerow(dict(zip(header,header)))

    for row in csv_in:
        json_res = run_faup(logger, row['url'])

        if json_res:
            row.update(json_res)

        csv_out.writerow(row)


main()

if __name__ == "__main__":
    main()
