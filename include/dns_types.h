#ifndef DNS_TYPES_H
#define DNS_TYPES_H

#include <stdint.h>

#define DNS_MAX_NAME_LENGTH 255
#define DNS_MAX_LABEL_LENGTH 63

typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} DNSHeader;

typedef struct {
    char* name;
    uint16_t type;
    uint16_t class;
} DNSQuestion;

typedef struct {
    char* name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t* rdata;
} DNSRecord;

#endif
