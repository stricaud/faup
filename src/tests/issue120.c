#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <faup/faup.h>
#include <faup/decode.h>

int main(){
    const char* url = "sub.sub.sub.domain.com/";

    char* domain = malloc(strlen(url)+1);
    memset(domain, 0, strlen(url)+1);
    strcpy(domain, url);

    faup_options_t* opts = faup_options_new();
    faup_handler_t* fh = faup_init(opts);

    faup_decode(fh, domain, strlen(domain));

    free(domain);
    faup_terminate(fh);
    faup_options_free(opts);
    return 0;
}
