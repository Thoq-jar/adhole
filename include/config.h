#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#ifndef __APPLE__
#include <stdbool.h>
#endif

typedef struct {
    char* upstream_dns;
    char* blocklist_file;
    char* listen_address;
    bool ipv6_enabled;

    uint16_t listen_port;
} DNSConfig;

DNSConfig* config_load(const char* filename);
void config_free(DNSConfig* config);

#endif
