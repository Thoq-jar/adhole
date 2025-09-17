#define GNU_SOURCE
#include "dns_types.h"
#include "dns_server.h"
#include <logger.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
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
    if (*offset >= maxlen)
        return nullptr;

    char name[DNS_MAX_NAME_LENGTH + 1];
    size_t name_len = 0;
    size_t current_offset = *offset;
    size_t jumps = 0;

    while (current_offset < maxlen) {
        const uint8_t len = src[current_offset++];

        if ((len & 0xC0) == 0xC0) { constexpr size_t max_jumps = 10;
            if (current_offset >= maxlen || jumps++ >= max_jumps)
                return nullptr;

            const size_t new_offset = (len & 0x3F) << 8 | src[current_offset++];
            if (new_offset >= maxlen)
                return nullptr;

            if (*offset == 0)
                *offset = current_offset;

            current_offset = new_offset;
            continue;
        }

        if (len == 0)
            break;

        if (name_len > 0) {
            if (name_len >= DNS_MAX_NAME_LENGTH)
                return nullptr;

            name[name_len++] = '.';
        }

        if (current_offset + len > maxlen || name_len + len > DNS_MAX_NAME_LENGTH)
            return nullptr;

        memcpy(name + name_len, src + current_offset, len);
        name_len += len;
        current_offset += len;
    }

    if (name_len >= DNS_MAX_NAME_LENGTH)
        return nullptr;

    name[name_len] = '\0';
    
    if (*offset == 0)
        *offset = current_offset;

    return strdup(name);
}

static void process_dns_query(const DNSServer *server, const uint8_t *query, const size_t query_len,
                              uint8_t *response, size_t *response_len) {
    if (!server || query_len < sizeof(DNSHeader) || query_len > DNS_MAX_PACKET_SIZE) {
        *response_len = 0;
        return;
    }

    memcpy(response, query, query_len);
    
    DNSHeader *header = (DNSHeader *) response;
    size_t offset = sizeof(DNSHeader);

    if ((header->flags & 0x8000) != 0 ||
        (header->flags & 0x7800) != 0) {
        *response_len = 0;
        return;
    }

    char *domain = dns_decode_name(query, &offset, query_len);
    if (!domain) {
        *response_len = 0;
        return;
    }

    size_t domain_len = strlen(domain);
    if (domain_len == 0 || domain_len > DNS_MAX_NAME_LENGTH) {
        free(domain);
        *response_len = 0;
        return;
    }

    if (blocklist_check_domain(server->blocklist, domain)) {
        header->flags |= 0x8183;
        *response_len = query_len;
    } else {
        struct sockaddr_in upstream = {0};
        upstream.sin_family = AF_INET;
        upstream.sin_port = htons(53);
        if (inet_pton(AF_INET, server->config->upstream_dns, &upstream.sin_addr) != 1) {
            free(domain);
            *response_len = 0;
            return;
        }

        const ssize_t sent = sendto(server->upstream_fd, query, query_len, 0,
                            (struct sockaddr *) &upstream, sizeof(upstream));
        
        if (sent < 0 || (size_t)sent != query_len) {
            free(domain);
            *response_len = 0;
            return;
        }

        struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
        setsockopt(server->upstream_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        *response_len = recvfrom(server->upstream_fd, response, DNS_MAX_PACKET_SIZE, 0, nullptr, nullptr);
        
        if (*response_len < sizeof(DNSHeader) || *response_len > DNS_MAX_PACKET_SIZE)
            *response_len = 0;
    }

    free(domain);
}

DNSServer *dns_server_create(const DNSConfig *config) {
    if(!config)
        return nullptr;

    DNSServer *server = calloc(1, sizeof(DNSServer));
    if(!server)
        return nullptr;

    server->blocklist = blocklist_init();
    if(!server->blocklist) {
        free(server);
        return nullptr;
    }

    if(!blocklist_load_file(server->blocklist, config->blocklist_file)) {
        logger_error("Failed to load blocklist from: %s", config->blocklist_file);
        blocklist_free(server->blocklist);
        free(server);

        return nullptr;
    }

    server->config = config;
    server->socket_fd = -1;
    server->upstream_fd = -1;

    return server;
}

[[noreturn]] void dns_server_start(DNSServer *server) {
    server->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server->socket_fd < 0) {
        dns_server_free(server);
        logger_error("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    server->upstream_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server->upstream_fd < 0) {
        dns_server_free(server);
        logger_error("Failed to create upstream socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server->config->listen_port);
    inet_pton(AF_INET, server->config->listen_address, &addr.sin_addr);

    if(bind(server->socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        dns_server_free(server);
        logger_error("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    server->running = true;
    struct sockaddr_in client;
    socklen_t client_len;

    while(true) {
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
}

void dns_server_stop(DNSServer *server) {
    if(!server)
        return;

    server->running = false;
}

void dns_server_free(DNSServer *server) {
    if(!server) return;

    if(server->socket_fd >= 0)
        close(server->socket_fd);
    if(server->upstream_fd >= 0)
        close(server->upstream_fd);
    if(server->blocklist)
        blocklist_free(server->blocklist);
    free(server);
}
