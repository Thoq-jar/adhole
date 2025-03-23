#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char* upstream_dns;
    char* blocklist_file;
    uint16_t listen_port;
    char* listen_address;
    bool ipv6_enabled;
} DNSConfig;

DNSConfig* config_load(const char* filename);
void config_free(DNSConfig* config);

#endif
