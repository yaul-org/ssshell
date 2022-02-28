#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>

#include "types.h"
#include "commands.h"

struct command;

typedef enum {
        OBJECT_TYPE_COMMAND,
        OBJECT_TYPE_SYMBOL,
        OBJECT_TYPE_STRING,
        OBJECT_TYPE_INTEGER,

        OBJECT_TYPE_COUNT,
} object_type_t;

struct object {
        object_type_t type;

        union {
                const command_t *command;
                char *symbol;
                char *string;
                int integer;
                void *value;
        } as;
};

typedef void (*object_destructor_func_t)(object_t *object);

object_t *object_new(object_type_t type);
void object_delete(object_t *object);

object_t *object_integer_new(int value);
object_t *object_string_new(char *value);
object_t *object_string_copy_new(char *value);
object_t *object_symbol_new(char *value);
object_t *object_symbol_copy_new(char *value);

void object_destructor_set(object_type_t type, object_destructor_func_t func);

#endif /* OBJECT_H */
