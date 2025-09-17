#include "dns_string.h"
#include <stdlib.h>
#include <string.h>

char *dns_strdup_safe(const char *str) {
    if(!str)
        return nullptr;

    const size_t len = strlen(str);
    char *dup = malloc(len + 1);

    if(dup) {
        memcpy(dup, str, len);
        dup[len] = '\0';
    }

    return dup;
}
