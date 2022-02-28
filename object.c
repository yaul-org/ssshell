#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "object.h"

static object_destructor_func_t _destructor_funcs[OBJECT_TYPE_COUNT];

void
object_destructor_set(object_type_t type, object_destructor_func_t func)
{
        assert(type != OBJECT_TYPE_COUNT);

        _destructor_funcs[type] = func;
}

object_t *
object_new(object_type_t type)
{
        assert(type != OBJECT_TYPE_COUNT);

        object_t * const object = malloc(sizeof(object_t));
        *object = (object_t) {
                .type     = type,
                .as.value = NULL
        };

        assert(object != NULL);

        return object;
}

object_t *
object_integer_new(int value)
{
        object_t * const object = object_new(OBJECT_TYPE_INTEGER);

        object->as.integer = value;

        return object;
}

object_t *
object_string_new(char *value)
{
        assert(value != NULL);

        object_t * const object = object_new(OBJECT_TYPE_STRING);

        object->as.string = value;

        return object;
}

object_t *
object_string_copy_new(char *value)
{
        assert(value != NULL);

        return object_string_new(strdup(value));
}

object_t *
object_symbol_new(char *value)
{
        assert(value != NULL);

        object_t * const object = object_new(OBJECT_TYPE_SYMBOL);

        object->as.symbol = value;

        return object;
}

object_t *
object_symbol_copy_new(char *value)
{
        assert(value != NULL);

        return object_symbol_new(strdup(value));
}

void
object_delete(object_t *object)
{
        if (object != NULL) {
                object_destructor_func_t const destructor_func =
                    _destructor_funcs[object->type];

                if (destructor_func != NULL) {
                        destructor_func(object);
                }

                free(object);
        }
}
