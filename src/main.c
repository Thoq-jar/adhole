#define GNU_SOURCE
#include "dns_server.h"
#include "config.h"
#include "logger.h"
#include <signal.h>
#include <string.h>

static DNSServer *g_server = nullptr;

static void signal_handler(const int signum) {
    (void) signum;

    if(g_server)
        dns_server_stop(g_server);
}

static void setup_signals(void) {
    struct sigaction sa = {};
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
}

int main(const int argc, char *argv[]) {
    if(argc != 2) {
        logger_error("Usage: adhole <config.conf>");
        return 1;
    }

    logger_adweb("Welcome!");
    logger_info("Starting DNS server...");

    DNSConfig *config = config_load(argv[1]);
    if(!config)
        return 1;

    setup_signals();

    g_server = dns_server_create(config);
    if(!g_server) {
        config_free(config);

        return 1;
    }

    dns_server_start(g_server);
    dns_server_free(g_server);
    config_free(config);

    return 0;
}
