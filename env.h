#ifndef ENV_H
#define ENV_H

#include <stddef.h>
#include <stdbool.h>

#include "types.h"

struct env_pair {
        const char *symbol;
        void *value;
};

void env_init(void);
void env_deinit(void);

void env_put(const char *symbol, void *value);
void *env_value_get(const char *symbol);
bool env_get(const char *symbol, env_pair_t *pair);

#endif /* ENV_H */
