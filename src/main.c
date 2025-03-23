#include "dns_server.h"
#include "config.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "logger.h"

static DNSServer* g_server = NULL;

static void signal_handler(int signum) {
    (void)signum;
    if (g_server) {
        dns_server_stop(g_server);
    }
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int main(const int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }

    logger_adhole("Welcome!");
    logger_info("Starting DNS server...");

    DNSConfig* config = config_load(argv[1]);
    if (!config) {
        return 1;
    }

    setup_signals();

    g_server = dns_server_create(config);
    if (!g_server) {
        config_free(config);
        return 1;
    }

    if (!dns_server_start(g_server)) {
        dns_server_free(g_server);
        config_free(config);
        return 1;
    }

    dns_server_free(g_server);
    config_free(config);
    return 0;
}
