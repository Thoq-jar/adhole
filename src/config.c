#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DNSConfig* config_load(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open config file: %s\n", filename);
        return NULL;
    }

    DNSConfig* config = calloc(1, sizeof(DNSConfig));
    if (!config) {
        fclose(file);
        return NULL;
    }

    config->listen_port = 53;
    config->listen_address = strdup("0.0.0.0");
    config->upstream_dns = strdup("1.1.1.1");
    config->blocklist_file = strdup("");
    config->ipv6_enabled = true;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        const char* key = line;
        char* value = eq + 1;

        char* nl = strchr(value, '\n');
        if (nl) *nl = '\0';

        if (strcmp(key, "listen_port") == 0) {
            if (strcmp(key, "listen_port") == 0) {
                char* endptr;
                const long port = strtol(value, &endptr, 10);

                if (endptr == value || *endptr != '\0' ||
                    port <= 0 || port > UINT16_MAX) {
                    fprintf(stderr, "Invalid port number: %s\n", value);
                    config->listen_port = 53;
                    } else {
                        config->listen_port = (uint16_t)port;
                    }
            }
        } else if (strcmp(key, "listen_address") == 0) {
            free(config->listen_address);
            config->listen_address = strdup(value);
        } else if (strcmp(key, "upstream_dns") == 0) {
            free(config->upstream_dns);
            config->upstream_dns = strdup(value);
        } else if (strcmp(key, "blocklist_file") == 0) {
            free(config->blocklist_file);
            config->blocklist_file = strdup(value);
        } else if (strcmp(key, "ipv6_enabled") == 0) {
            config->ipv6_enabled = (strcmp(value, "true") == 0);
        }
    }

    fclose(file);
    return config;
}

void config_free(DNSConfig* config) {
    if (!config) return;
    free(config->upstream_dns);
    free(config->blocklist_file);
    free(config->listen_address);
    free(config);
}
