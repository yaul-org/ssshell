#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "env.h"

#include <sys/queue.h>

struct env_entry;

typedef TAILQ_HEAD(environment, env_entry) environment_t;

typedef struct env_entry env_entry_t;

struct env_entry {
        env_pair_t env_pair;

        TAILQ_ENTRY(env_entry) entries;
};

static environment_t _environment;

static env_entry_t *_env_entry_get(const char *symbol);

void
env_init(void)
{
        TAILQ_INIT(&_environment);
}

void
env_deinit(void)
{
        env_entry_t *env_np;
        env_np = TAILQ_FIRST(&_environment);

        while (env_np != NULL) {
                env_entry_t * const next =
                    TAILQ_NEXT(env_np, entries);

                if (env_np->env_pair.symbol != NULL) {
                        free((void *)env_np->env_pair.symbol);
                }

                free(env_np);

                env_np = next;
        }
}

void
env_put(const char *symbol, void *value)
{
        assert(symbol != NULL);

        env_entry_t *env_entry;

        env_entry = _env_entry_get(symbol);
        if (env_entry != NULL) {
                env_entry->env_pair.value = value;
        } else {
                env_entry = malloc(sizeof(env_entry_t));
                assert(env_entry != NULL);

                env_entry->env_pair.symbol = strdup(symbol);
                env_entry->env_pair.value = value;

                TAILQ_INSERT_TAIL(&_environment, env_entry, entries);
        }
}

bool
env_get(const char *symbol, env_pair_t *pair)
{
        assert(symbol != NULL);
        assert(pair != NULL);

        env_entry_t * const env_entry = _env_entry_get(symbol);

        pair->symbol = symbol;
        pair->value = NULL;

        if (env_entry == NULL) {
                return false;
        }

        pair->value = env_entry->env_pair.value;

        return true;
}

void *
env_value_get(const char *symbol)
{
        env_pair_t pair;

        if (!(env_get(symbol, &pair))) {
                return NULL;
        }

        return pair.value;
}

static env_entry_t *
_env_entry_get(const char *symbol)
{
        env_entry_t *env_np;
        TAILQ_FOREACH (env_np, &_environment, entries) {
                if ((strcmp(symbol, env_np->env_pair.symbol)) == 0) {
                        return env_np;
                }
        }

        return NULL;
}
