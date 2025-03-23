#include "blocklist.h"

#include <ctype.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BlockList {
    char **domains;
    size_t count;
};

bool blocklist_load_file(BlockList *list, const char *filename) {
    if(!list || !filename) return false;

    FILE *file = fopen(filename, "r");
    if(!file) return false;

    size_t count = 0;
    char line[1024];

    while(fgets(line, sizeof(line), file)) {
        if(line[0] == '#' || line[0] == '\n') continue;

        if(strstr(line, "localhost") != NULL) continue;

        char ip[64], domain[256];
        if(sscanf(line, "%s %s", ip, domain) == 2) {
            count++;
        }
    }

    list->domains = calloc(count, sizeof(char *));
    if(!list->domains) {
        fclose(file);
        return false;
    }

    rewind(file);

    size_t index = 0;
    while(fgets(line, sizeof(line), file) && index < count) {
        if(line[0] == '#' || line[0] == '\n') continue;

        char ip[64], domain[256];
        if(sscanf(line, "%s %s", ip, domain) == 2) {
            if(strchr(domain, '.') && isdigit(domain[0]) &&
               isdigit(domain[strlen(domain) - 1])) {
                continue;
            }

            list->domains[index] = strdup(domain);
            if(!list->domains[index]) {
                for(size_t i = 0; i < index; i++) {
                    free(list->domains[i]);
                }
                free(list->domains);
                list->domains = NULL;
                list->count = 0;
                fclose(file);
                return false;
            }
            index++;
        }
    }

    list->count = index;
    fclose(file);

    logger_info
            ("Loaded %zu domains into blocklist\n", list->count);
    return true;
}

BlockList *blocklist_init(void) {
    BlockList *list = calloc(1, sizeof(BlockList));
    return list;
}

bool blocklist_check_domain(const BlockList *list, const char *domain) {
    if(!list || !domain) return false;

    logger_seperator();
    logger_adhole("Screening domain: %s", domain);
    for(size_t i = 0; i < list->count; i++) {
        if(strcasecmp(list->domains[i], domain) == 0) {
            logger_adhole("Domain %s found in blocklist", domain);
            logger_blocked("%s", domain);
            logger_n_line();
            return true;
        }
    }

    logger_adhole("Domain %s not found in blocklist", domain);
    logger_allowed("%s", domain);
    logger_n_line();

    return false;
}

void blocklist_free(BlockList *list) {
    if(!list) return;

    for(size_t i = 0; i < list->count; i++) {
        free(list->domains[i]);
    }
    free(list->domains);
    free(list);
}
