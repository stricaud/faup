#!/usr/bin/env python

import csv
import sys
import os
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
		try:
			faup_r = envoy.run("echo %s |faup" % (url_value))
			faup_url_results = faup_r.std_out.rstrip().split(',')

			# Make sure empty stuff are not empty
			counter = 0
			while (counter < len(faup_url_results)):
				if len(faup_url_results[counter]) == 0:
					faup_url_results[counter] = " "
				counter += 1

			result[SchemeColumn]       = faup_url_results[0]
			result[CredentialColumn]   = faup_url_results[1]
			result[SubdomainColumn]    = faup_url_results[2]
			result[DomainColumn]       = faup_url_results[3]
			result[HostColumn]         = faup_url_results[4]
			result[TLDColumn]          = faup_url_results[5]
			result[PortColumn]         = faup_url_results[6]
			result[ResourcePathColumn] = faup_url_results[7]
			result[QueryStringColumn]  = faup_url_results[8]
			result[FragmentColumn]     = faup_url_results[9]

		except:
			logger.error("Cannot normalize url: %s" % (url_value))

		writer.writerow(result)
	else:
		writer.writerow(result)


main()
