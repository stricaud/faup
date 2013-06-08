#!/usr/bin/env python

import csv
import sys
import os
import json
import logging, logging.handlers
import envoy

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
	run_command = 'echo -n "%s" |faup -o json' % (url_value)
	try:
		faup_r = envoy.run(run_command)
		json_from_faup = faup_r.std_out
	except:
		logger.info("Error with: %s" % run_command)
		return None

	try:
		json_tree = json.loads(json_from_faup)
		return json_tree
	except:
		logger.info("ERROR TO LOAD JSON: %s" % (json_from_faup))

	return None


def main():
    logger = setup_logger()

    # Get url column
    if len(sys.argv) < 2:
        print("No argument provided")
        sys.exit(0)

    urlColumn = sys.argv[1]

    # scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
    SchemeColumn       = 'url_scheme'
    CredentialColumn   = 'url_credential'
    SubdomainColumn    = 'url_subdomain'
    DomainColumn       = 'url_domain'
    HostColumn         = 'url_host'
    TLDColumn          = 'url_tld'
    PortColumn         = 'url_port'
    ResourcePathColumn = 'url_resource_path'
    QueryStringColumn  = 'url_query_string'
    FragmentColumn     = 'url_fragment'
    ErrorColumn        = 'url_error'

    reader = csv.reader(sys.stdin)
    writer = None
    header = []

    first = True
    for line in reader:
        if first:
            header = line

            if urlColumn not in header:
                logger.warn("URL not provided")
                sys.exit(0)

            csv.writer(sys.stdout).writerow(header)
            writer = csv.DictWriter(sys.stdout, header)
            first = False
            continue

        result = {}
        i = 0
        #loop through the list of headers
        while i < len(header):
            if i < len(line):
                result[header[i]] = line[i]
            else:
                result[header[i]] = ''
            i += 1

	url_value = result[urlColumn]
	faup_url_results = []

	# If we have an URL, split it using faup
        if len(result[urlColumn]):
		json_tree = run_faup(logger, url_value)

		if json_tree is None:
			writer.writerow(result)
			return

		result[SchemeColumn]       = json_tree['scheme']
		result[CredentialColumn]   = json_tree['credential']
		result[SubdomainColumn]    = json_tree['subdomain']
		result[DomainColumn]       = json_tree['domain']
		result[HostColumn]         = json_tree['host']
		result[TLDColumn]          = json_tree['tld']
		result[PortColumn]         = json_tree['port']
		result[ResourcePathColumn] = json_tree['resource_path']
		result[QueryStringColumn]  = json_tree['query_string']
		result[FragmentColumn]     = json_tree['fragment']
			
		writer.writerow(result)
	else:
		writer.writerow(result)


main()
