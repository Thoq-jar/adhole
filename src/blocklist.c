#define GNU_SOURCE
#include "blocklist.h"
#include <ctype.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>

#ifndef __APPLE__
#include <string.h>
#include <stdint.h>
#endif

#if !defined(HAVE_REALLOCARRAY)
static void *reallocarray(void *ptr, const size_t nmemb, size_t size) {
    if (nmemb > SIZE_MAX / size) {
        errno = ENOMEM;
        return NULL;
    }
    return realloc(ptr, nmemb * size);
}
#endif

struct BlockList {
    char **domains;
    size_t count;
    size_t capacity;
};

#define INITIAL_CAPACITY 1000
#define GROWTH_FACTOR 2

static bool blocklist_ensure_capacity(BlockList *list, size_t needed) {
    if (needed <= list->capacity) return true;

    size_t new_capacity = list->capacity ? list->capacity * GROWTH_FACTOR : INITIAL_CAPACITY;
    while (new_capacity < needed) {
        new_capacity *= GROWTH_FACTOR;
    }

    char **new_domains = reallocarray(list->domains, new_capacity, sizeof(char *));
    if (!new_domains) return false;

    list->domains = new_domains;
    list->capacity = new_capacity;
    return true;
}

bool blocklist_load_file(BlockList *list, const char *filename) {
    if (!list || !filename) return false;

    FILE *file = fopen(filename, "r");
    if (!file) return false;

    char line[1024];
    size_t loaded = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (strstr(line, "localhost") != NULL) continue;

        char ip[64], domain[256];
        if (sscanf(line, "%63s %255s", ip, domain) != 2) continue;

        if (strchr(domain, '.') && isdigit(domain[0]) &&
            isdigit(domain[strlen(domain) - 1])) {
            continue;
        }

        if (!blocklist_ensure_capacity(list, loaded + 1)) {
            fclose(file);
            return false;
        }

        list->domains[loaded] = strdup(domain);
        if (!list->domains[loaded]) {
            fclose(file);
            return false;
        }
        loaded++;
    }

    list->count = loaded;
    fclose(file);

    logger_info("Loaded %zu domains into blocklist\n", list->count);
    return true;
}

BlockList *blocklist_init(void) {
    BlockList *list = calloc(1, sizeof(BlockList));
    if (!list) return nullptr;

    if (!blocklist_ensure_capacity(list, INITIAL_CAPACITY)) {
        free(list);
        return nullptr;
    }

    return list;
}

bool blocklist_check_domain(const BlockList *list, const char *domain) {
    if (!list || !domain) return false;

    logger_seperator();
    logger_adweb("Screening domain: %s", domain);

    for (size_t i = 0; i < list->count; i++) {
        if (strcasecmp(list->domains[i], domain) == 0) {
            logger_adweb("Domain %s found in blocklist", domain);
            logger_blocked("%s", domain);
            logger_n_line();
            return true;
        }
    }

    logger_adweb("Domain %s not found in blocklist", domain);
    logger_allowed("%s", domain);
    logger_n_line();

    return false;
}

void blocklist_free(BlockList *list) {
    if (!list) return;

    for (size_t i = 0; i < list->count; i++) {
        free(list->domains[i]);
    }
    free(list->domains);
    free(list);
}
