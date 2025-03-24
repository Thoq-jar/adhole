#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#include <stdbool.h>
#include "dns_types.h"
#include "config.h"
#include "blocklist.h"

typedef struct DNSServer DNSServer;

DNSServer* dns_server_create(const DNSConfig* config);
bool dns_server_start(DNSServer* server);
void dns_server_stop(DNSServer* server);
void dns_server_free(DNSServer* server);

#endif
