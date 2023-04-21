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
		char *out = malloc(size + 1);
		memset(out, '\0', size+1);
		memcpy(out, instr, size);

		printf("The decoded scheme is '%s'\n", out);
		free(out);
	}

	faup_terminate(fh);

	return 0;
}
