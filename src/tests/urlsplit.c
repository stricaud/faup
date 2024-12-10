#include <stdio.h>
#include <stdlib.h>

#include <faup/faup.h>
#include <faup/decode.h>

int main(int argc, char **argv)
{
	faup_handler_t *fh;
	const char *instr = "http://www.slashdot.org/";
	size_t pos, size;

	fh = faup_init(NULL);

	faup_decode(fh, instr, strlen(instr));

	pos = faup_get_scheme_pos(fh);
	size = faup_get_scheme_size(fh);
	if (size > 0) {
		printf("scheme:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_credential_pos(fh);
	size = faup_get_credential_size(fh);
	if (size > 0) {
		printf("credential:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_subdomain_pos(fh);
	size = faup_get_subdomain_size(fh);
	if (size > 0) {
		printf("subdomain:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_domain_pos(fh);
	size = faup_get_domain_size(fh);
	if (size > 0) {
		printf("domain:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_domain_without_tld_pos(fh);
	size = faup_get_domain_without_tld_size(fh);
	if (size > 0) {
		printf("domain without tld:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_host_pos(fh);
	size = faup_get_host_size(fh);
	if (size > 0) {
		printf("host:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_tld_pos(fh);
	size = faup_get_tld_size(fh);
	if (size > 0) {
		printf("tld:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_port_pos(fh);
	size = faup_get_port_size(fh);
	if (size > 0) {
		printf("port:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_resource_path_pos(fh);
	size = faup_get_resource_path_size(fh);
	if (size > 0) {
		printf("resource path:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_query_string_pos(fh);
	size = faup_get_query_string_size(fh);
	if (size > 0) {
		printf("query string:%.*s\n", (int)size, instr + pos);
	}
	pos = faup_get_fragment_pos(fh);
	size = faup_get_fragment_size(fh);
	if (size > 0) {
		printf("fragment:%.*s\n", (int)size, instr + pos);
	}

	faup_terminate(fh);

	return 0;
}
