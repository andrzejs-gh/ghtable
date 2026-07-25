#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "ghtable.h"
#include "time_exec.h"

#define LIMIT 1000000

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

size_t count = 0;
bool global_check_list[LIMIT] = {false};

static inline bool is_entry_valid(const char* str_key, size_t value)
{
    size_t val_from_key = (size_t)( strtoull(&str_key[4], NULL, 10) );
    if ( val_from_key != value )
        return false;

    return true;
}

static inline void count_ver_and_inc(ghtable* ght, size_t key_number)
{
    if ( ghtable_count(ght) != count )
    {
        puts(ERROR " Table holds incorect count!");
        assert(false);
    }

    if ( !global_check_list[key_number] )
        count++;
}

static inline void count_ver_and_dec(ghtable* ght, size_t key_number)
{
    if ( ghtable_count(ght) != count )
    {
        puts(ERROR " Table holds incorect count!");
        assert(false);
    }

    if ( global_check_list[key_number] )
        count--;
}

static inline size_t random_from_range(range r)
{
    return r.min + random() % (r.max - r.min);
}

static inline char* get_rand_pair(char* key, size_t* value)
{
    // size_t key_number = random_from_range(r);
    size_t key_number = random() % LIMIT;
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

static inline size_t* get_rand_pairn(size_t* key, size_t* value)
{
    // size_t key_number = random_from_range(r);
    size_t key_number = random() % LIMIT;
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

void set_random(ghtable* ght)
{
    const void* ret;
    double t;

    if ( random() % 2 == 0 )
    {
        get_rand_pair(key, &value);
        count_ver_and_inc(ght, value);

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
        get_rand_pairn(&bkey, &value);
        count_ver_and_inc(ght, value);

        TIME_STORE( t,
                    ret = ghtable_setn(ght, &bkey, sizeof bkey, &value, sizeof value);
        );
        if ( ret )
            printf(OK " Set: %zu : %zu in t = %.9f s \n", bkey, value, t);
        else
        {
            printf(ERROR " Failed to set %zu : %zu \n", bkey, value);
            assert(false);
        }
    }
    global_check_list[value] = true;
}

void get_random(ghtable* ght)
{
    const size_t* value_ptr;
    double t;

    if ( random() % 2 == 0 )
    {
        get_rand_pair(key, &value);

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
        else if ( global_check_list[value] )
        {
            printf(ERROR " Failed to retrieve \"%s\" \n", key);
            assert(false);
        }
    }
    else
    {
        get_rand_pairn(&bkey, &value);

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
        else if ( global_check_list[value] )
        {
            printf(ERROR " Failed to retrieve %zu : %zu \n", bkey, value);
            assert(false);
        }
    }
}

void del_random(ghtable* ght)
{
    int ret;
    double t;

    if ( random() % 2 == 0 )
    {
        get_rand_pair(key, &value);
        count_ver_and_dec(ght, value);

        TIME_STORE( t,
                    ret = ghtable_del(ght, key);
        );
        if ( !ret )
            printf(OK " Deleted: \"%s\" : %zu in t = %.9f s \n", key, value, t);
        else if ( global_check_list[value] )
        {
            printf(ERROR " Failed to delete \"%s\" : %zu \n", key, value);
            ret == ENTRY_NOT_FOUND ? puts(ERROR " ENTRY_NOT_FOUND") : puts(ERROR " TABLE_OR_KEY_NULL");
            assert(false);
        }
    }
    else
    {
        get_rand_pairn(&bkey, &value);
        count_ver_and_dec(ght, value);

        TIME_STORE( t,
                    ret = ghtable_deln(ght, &bkey, sizeof bkey);
        );
        if ( !ret )
            printf(OK " Deleted: %zu : %zu in t = %.9f s \n", bkey, value, t);
        else if ( global_check_list[value] )
        {
            printf(ERROR " Failed to delete %zu : %zu \n", bkey, value);
            ret == ENTRY_NOT_FOUND ? puts(ERROR " ENTRY_NOT_FOUND") : puts(ERROR " TABLE_OR_KEY_NULL");
            assert(false);
        }
    }
    global_check_list[value] = false;
}

void random_nth(ghtable* ght)
{
    size_t ght_count = ghtable_count(ght);
    if ( !ght_count ) return;

    size_t n = random() % ght_count;
    key_list_entry kl_entry = ghtable_nth_kl_entry(ght, n);
    if ( !kl_entry.key )
    {
        printf(ERROR " Key number %zu not found on the key list! \n", n);
        assert(false);
    }

    const size_t* val = ghtable_nth(ght, n);
    if ( !val )
    {
        printf(ERROR " Key number %zu not found in the table! \n", n);
        assert(false);
    }
    else if ( *val % 2 == 0 )
    {
        if ( !is_entry_valid(kl_entry.key, *val) )
        {
            printf(ERROR " Invalid entry: \"%s\" : " RED("%zu") "\n",
                                            (char*)kl_entry.key, *val);
            assert(false);
        }
    }
    else
    {
        if ( *(size_t*)kl_entry.key != *val )
        {
            printf(ERROR " Invalid entry: %zu : " RED("%zu") "\n",
                   *(size_t*)kl_entry.key, *val);
            assert(false);
        }
    }

    printf(OK " Entry number %zu verified \n", n);
}

void fuzzy_test(ghtable* ght)
{
    for ( size_t i = 0; i < LIMIT; i++ )
    {
        size_t r = random_from_range((range){0, 5});
        switch ( r )
        {
            case 0: set_random(ght); break;
            case 1: get_random(ght); break;
            case 2: del_random(ght); break;
            case 3: random_nth(ght); break;
            case 4: break;
        }
    }
}

int main(void)
{
    srandom((unsigned)time(NULL));

    ghtable* ght = new_ghtable(1, ORD);
    fuzzy_test(ght);

    return 0;
}
