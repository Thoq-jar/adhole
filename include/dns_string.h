#ifndef DNS_STRING_H
#define DNS_STRING_H

#include <stdbool.h>
#include <stddef.h>

char* dns_strdup_safe(const char* str);
size_t dns_strlcpy(char* dst, const char* src, size_t size);
bool dns_validate_domain_name(const char* domain);

#endif
