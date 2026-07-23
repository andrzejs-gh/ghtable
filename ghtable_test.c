#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "ghtable.h"
#include "time_exec.h"

#define GHT_COUNT 1000000
#define RED(x) "\033[31m" x "\033[0m"
#define GREEN(x) "\033[32m" x "\033[0m"

#define OK "[ " GREEN("OK") " ]"
#define ERROR "[ " RED("ERROR") " ]"

typedef struct
{
    size_t min;
    size_t max;

} range;

char key[32];
size_t bkey;
size_t value;
size_t deleted[GHT_COUNT / 2];

static inline size_t random_from_range(range r)
{
    return r.min + random() % (r.max - r.min);
}

void insert_elements(ghtable* ght, size_t n)
{
    bool is_even = true;
    char buffer[32];

    for ( size_t i = 0; i < n; i++ )
    {
        if ( is_even )
        {
            sprintf(buffer, "key %zu", i);
            if ( !ghtable_set(ght, buffer, &i, sizeof i) )
            {
                printf(ERROR " Failed insertion at %zu\n", i);
                assert(false);
            }
            is_even = false;
        }
        else
        {
            if ( !ghtable_setn(ght, &i, sizeof i, &i, sizeof i) )
            {
                printf(ERROR " Failed insertion at %zu\n", i);
                assert(false);
            }
            is_even = true;
        }
    }
}

static inline char* get_rand_pair(char* key, size_t* value, range r)
{
    size_t key_number = random_from_range(r);
    if ( key_number % 2 != 0 )
        key_number--;

    if ( sprintf(key, "key %zu", key_number) > 0 )
    {
        *value = key_number;
        return key;
    }
    else
        return NULL;
}

static inline size_t* get_rand_pairn(size_t* key, size_t* value, range r)
{
    size_t key_number = random_from_range(r);
    if ( key_number % 2 == 0 )
    {
        if ( key_number == 0 )
            key_number++;
        else
            key_number--;
    }

    *key = key_number;
    *value = key_number;

    return key;
}

void set_random(ghtable* ght, range r)
{
    const void* ret;
    double t;

    if ( random() % 2 == 0 )
    {
        get_rand_pair(key, &value, r);

        TIME_STORE( t,
                    ret = ghtable_set(ght, key, &value, sizeof value);
        );
        if ( ret )
            printf(OK " Set: \"%s\" : %zu in t = %.9f s \n", key, value, t);
        else
        {
            printf(ERROR " Failed to set \"%s\" : %zu \n", key, value);
            assert(false);
        }
    }
    else
    {
        get_rand_pairn(&bkey, &value, r);

        TIME_STORE( t,
                    ret = ghtable_setn(ght, key, sizeof bkey, &value, sizeof value);
        );
        if ( ret )
            printf(OK " Set: %zu : %zu in t = %.9f s \n", bkey, value, t);
        else
        {
            printf(ERROR " Failed to set %zu : %zu \n", bkey, value);
            assert(false);
        }
    }
}

void get_random(ghtable* ght, range r)
{
    const size_t* value_ptr;
    double t;

    if ( random() % 2 == 0 )
    {
        get_rand_pair(key, &value, r);

        TIME_STORE( t,
                    value_ptr = ghtable_get(ght, key);
        );
        if ( value_ptr )
        {
            if ( *value_ptr == value )
                printf(OK " Retrieved: \"%s\" : %zu in t = %.9f s \n",
                       key, value, t);
            else
            {
                printf(ERROR " Incorect value: \"%s\" : " RED("%zu\n"),
                       key, value);
                assert(false);
            }
        }
        else
        {
            printf(ERROR " Failed to retrieve \"%s\" \n", key);
            assert(false);
        }
    }
    else
    {
        get_rand_pairn(&bkey, &value, r);

        TIME_STORE( t,
                    value_ptr = ghtable_getn(ght, &bkey, sizeof bkey);
        );
        if ( value_ptr )
        {
            if ( *value_ptr == value )
                printf(OK " Retrieved: %zu : %zu in t = %.9f s \n",
                        bkey, value, t);
            else
            {
                printf(ERROR " Incorect value: %zu : " RED("%zu\n"),
                        bkey, value);
                assert(false);
            }
        }
        else
        {
            printf(ERROR " Failed to retrieve %zu : %zu \n", bkey, value);
            assert(false);
        }
    }
}

void del_random(ghtable* ght, range r)
{

}

void random_nth(ghtable* ght, range r)
{

}

int main(void)
{
    srandom((unsigned)time(NULL));

    size_t initial_count = GHT_COUNT / 2;

    ghtable* ght = new_ghtable(1, ORD);
    insert_elements(ght, initial_count);

    for ( int i = 0; i < 100; i++ )
        get_random(ght, (range){0, GHT_COUNT});

    return 0;
}
