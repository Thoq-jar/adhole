#include "dns_server.h"

#include <logger.h>

#include "dns_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct DNSServer {
    int socket_fd;
    int upstream_fd;
    BlockList *blocklist;
    const DNSConfig *config;
    bool running;
};

static char *dns_decode_name(const uint8_t *src, size_t *offset, const size_t maxlen) {
    char name[DNS_MAX_NAME_LENGTH + 1];
    size_t name_len = 0;
    size_t current_offset = *offset;

    while(current_offset < maxlen) {
        const uint8_t len = src[current_offset++];
        if(len == 0) break;

        if(name_len > 0) {
            name[name_len++] = '.';
        }

        if(current_offset + len > maxlen || name_len + len > DNS_MAX_NAME_LENGTH) {
            return NULL;
        }

        memcpy(name + name_len, src + current_offset, len);
        name_len += len;
        current_offset += len;
    }

    name[name_len] = '\0';
    *offset = current_offset;
    return strdup(name);
}

static void process_dns_query(const DNSServer *server, const uint8_t *query, const size_t query_len,
                              uint8_t *response, size_t *response_len) {
    if(query_len < sizeof(DNSHeader) || query_len > 255) {
        *response_len = 0;
        return;
    }

    memcpy(response, query, query_len);
    DNSHeader *header = (DNSHeader *) response;
    size_t offset = sizeof(DNSHeader);

    char *domain = dns_decode_name(query, &offset, query_len);
    if(!domain) {
        *response_len = 0;
        return;
    }

    if(blocklist_check_domain(server->blocklist, domain)) {
        header->flags |= 0x8183;
        *response_len = query_len;
    } else {
        struct sockaddr_in upstream = {0};
        upstream.sin_family = AF_INET;
        upstream.sin_port = htons(53);
        inet_pton(AF_INET, server->config->upstream_dns, &upstream.sin_addr);

        sendto(server->upstream_fd, query, query_len, 0,
               (struct sockaddr *) &upstream, sizeof(upstream));

        *response_len = recvfrom(server->upstream_fd, response, 512, 0, NULL, NULL);
    }

    free(domain);
}

DNSServer *dns_server_create(const DNSConfig *config) {
    if(!config) return NULL;

    DNSServer *server = calloc(1, sizeof(DNSServer));
    if(!server) return NULL;

    server->blocklist = blocklist_init();
    if(!server->blocklist) {
        free(server);
        return NULL;
    }

    if(!blocklist_load_file(server->blocklist, config->blocklist_file)) {
        logger_error("Failed to load blocklist from: %s", config->blocklist_file);
        blocklist_free(server->blocklist);
        free(server);
        return NULL;
    }

    server->config = config;
    server->socket_fd = -1;
    server->upstream_fd = -1;

    return server;
}

// ReSharper disable once CppDFAConstantFunctionResult
bool dns_server_start(DNSServer *server) {
    server->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server->socket_fd < 0) return false;

    server->upstream_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server->upstream_fd < 0) {
        close(server->socket_fd);
        return false;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server->config->listen_port);
    inet_pton(AF_INET, server->config->listen_address, &addr.sin_addr);

    if(bind(server->socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(server->socket_fd);
        close(server->upstream_fd);
        return false;
    }

    server->running = true;
    struct sockaddr_in client;
    socklen_t client_len;

    // ReSharper disable once CppDFAConstantConditions
    // ReSharper disable once CppDFAEndlessLoop
    while(server->running) {
        uint8_t query[512];
        client_len = sizeof(client);
        const ssize_t query_len = recvfrom(server->socket_fd, query, sizeof(query), 0,
                                           (struct sockaddr *) &client, &client_len);

        if(query_len > 0) {
            uint8_t response[512];
            size_t response_len;
            process_dns_query(server, query, query_len, response, &response_len);

            if(response_len > 0) {
                sendto(server->socket_fd, response, response_len, 0,
                       (struct sockaddr *) &client, client_len);
            }
        }
    }

    // ReSharper disable once CppDFAUnreachableCode
    return true;
}

void dns_server_stop(DNSServer *server) {
    if(!server) return;
    server->running = false;
}

void dns_server_free(DNSServer *server) {
    if(!server) return;

    if(server->socket_fd >= 0) {
        close(server->socket_fd);
    }
    if(server->upstream_fd >= 0) {
        close(server->upstream_fd);
    }
    if(server->blocklist) {
        blocklist_free(server->blocklist);
    }
    free(server);
}
