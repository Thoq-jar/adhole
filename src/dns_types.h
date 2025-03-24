#ifndef DNS_TYPES_H
#define DNS_TYPES_H

#include <stdint.h>

#define DNS_MAX_NAME_LENGTH 255
#define DNS_MAX_PACKET_SIZE 512

typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed)) DNSHeader;

#endif // DNS_TYPES_H
