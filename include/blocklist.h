#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#ifndef __APPLE__
#include <stdbool.h>
#endif

typedef struct BlockList BlockList;

BlockList* blocklist_init(void);
bool blocklist_load_file(BlockList* list, const char* filename);
bool blocklist_check_domain(const BlockList* list, const char* domain);
void blocklist_free(BlockList* list);

#endif
