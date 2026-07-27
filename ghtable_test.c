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

size_t count = 0;
bool global_check_list[LIMIT] = {false};
bool deletions_list[LIMIT] = {false};
size_t deletion_count = 0;

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

void set_random(ghtable* ght)
{
    const void* ret;
    double t;

    size_t key_number = random() % LIMIT;
    sprintf(key, "key %zu", key_number);

    TIME_STORE( t,
                ret = ghtable_set(ght, key, &key_number, sizeof key_number);
    );
    if ( ret )
        printf(OK " Set: \"%s\" : %zu in t = %.9f s \n", key, key_number, t);
    else
    {
        printf(ERROR " Failed to set \"%s\" : %zu \n", key, key_number);
        assert(false);
    }

    global_check_list[key_number] = true;
}

void get_random(ghtable* ght)
{
    const size_t* value_ptr;
    double t;

    size_t key_number;
    sprintf(key, "key %zu", key_number);

    TIME_STORE( t,
                value_ptr = ghtable_get(ght, key);
    );
    if ( value_ptr )
    {
        if ( *value_ptr == key_number )
            printf(OK " Retrieved: \"%s\" : %zu in t = %.9f s \n",
                   key, key_number, t);
        else
        {
            printf(ERROR " Incorect value: \"%s\" : " RED("%zu\n"),
                    key, key_number);
            assert(false);
        }
    }
    else if ( global_check_list[key_number] )
    {
        printf(ERROR " Failed to retrieve \"%s\" \n", key);
        assert(false);
    } //if ( !value_ptr ) { puts("NULL"); }

}

void del_random(ghtable* ght)
{
    int ret;
    double t;

    size_t key_number;
    sprintf(key, "key %zu", key_number);

    TIME_STORE( t,
                ret = ghtable_del(ght, key);
    );
    if ( !ret )
    {
        printf(OK " Deleted: \"%s\" : %zu in t = %.9f s \n", key, key_number, t);
        deletion_count++;
    }
    else if ( global_check_list[key_number] )
    {
        printf(ERROR " Failed to delete \"%s\" : %zu \n", key, key_number);
        ret == ENTRY_NOT_FOUND ? puts(ERROR " ENTRY_NOT_FOUND") : puts(ERROR " TABLE_OR_KEY_NULL");
        assert(false);
    }

    global_check_list[key_number] = false;
}

void random_nth(ghtable* ght)
{
    if ( !ghtable_count(ght) ) return;
    size_t n = random() % ghtable_count(ght);
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
    else if ( !is_entry_valid(key, *val) )
    {
        printf(ERROR " Invalid entry: \"%s\" : " RED("%zu") "\n",
               (char*)kl_entry.key, *val);
        assert(false);
    }

    printf(OK " Entry number %zu verified \n", n);
}

void shrink_table(ghtable* ght)
{
    size_t opt_size = ghtable_opt_size(ght);
    size_t key_list_opt_size = ghtable_key_list_opt_size(ght);
    bool failure = false;

    void* ret = ghtable_shrink(ght);
    if ( !ret )
    {
        puts(ERROR " ghtable_shrink failure. Could be realloc failure.");
        assert(false);
    }

    if ( ghtable_size(ght) != opt_size )
    {
        printf(ERROR " Table size mismatch after shrinking: "
                     "opt_size = %zu vs ghtable_size = %zu \n",
                     opt_size, ghtable_size(ght));
        failure = true;
    }
    if ( ghtable_key_list_size(ght) != key_list_opt_size )
    {
        printf(ERROR " Keylist size mismatch after table shrinking: "
                     "key_list_opt_size = %zu vs key_list_size = %zu \n",
                     key_list_opt_size, ghtable_key_list_size(ght));
        failure = true;
    }

    if ( failure )
        assert(false);
    else
        puts(OK " Table shrunk correctly.");
}

void fuzzy_test(ghtable* ght)
{
    range func_range = (range){0, 5};

    for ( size_t i = 0; i < LIMIT; i++ )
    {
        size_t r = random_from_range(func_range);
        switch ( r )
        {
            case 0: shrink_table(ght); break;
            case 1: get_random(ght); break;
            case 2: del_random(ght); break;
            case 3: random_nth(ght); break;
            case 4: break;
        }
        set_random(ght);
    }
}



int main(void)
{
    srandom((unsigned)time(NULL));

    ghtable* ght = new_ghtable(1, ORD);
    fuzzy_test(ght);


    return 0;
}
