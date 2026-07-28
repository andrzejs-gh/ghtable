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

size_t count_holes_in_kl(ghtable* ght);
void count_holes_in_table(ghtable* ght);

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

    size_t key_number = random() % LIMIT;
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

size_t get_ord_index(ghtable* ght, const char* key)
{
    const key_list_entry* keys = ghtable_key_list(ght);
    size_t ght_count = ghtable_count(ght);
    size_t key_len = strlen(key);

    for ( size_t i = 0; i < ght_count; i++ )
    {
        if ( !memcmp(key, keys[i].key, key_len) )
            return i;
    }

    return LIMIT+1;
}

void del_random(ghtable* ght)
{
    int ret;
    double t;

    size_t key_number = random() % LIMIT;
    sprintf(key, "key %zu", key_number);
    //
    //size_t ord_index = get_ord_index(ght, key);
    //printf("cap = %zu \n", ghtable_capacity(ght));
    //

    TIME_STORE( t,
                ret = ghtable_del(ght, key);
    );
    if ( !ret )
    {
        printf(OK " Deleted: \"%s\" : %zu in t = %.9f s \n", key, key_number, t);
        deletion_count++;
        //
        //size_t holes = count_holes_in_kl(ght);
        //printf("... and the ord index of deleted entry... %zu \n", ord_index);
        //printf("ght count = %zu \n", ghtable_count(ght));
        // if ( holes )
        //     assert(false);
        //
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
    if ( !ghtable_count(ght) || !ghtable_key_list(ght) ) return;

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
        //
        count_holes_in_kl(ght);
        count_holes_in_table(ght);
        //
        assert(false);
    }
    else if ( !is_entry_valid(kl_entry.key, *val) )
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
    if ( !ret && ghtable_count(ght) )
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
    if ( ghtable_key_list(ght) && ghtable_key_list_size(ght) != key_list_opt_size )
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

void count_holes_in_table(ghtable* ght)
{
    const size_t* ret;
    size_t count = 0;

    for ( size_t i = 0; i < LIMIT; i++ )
    {
        if ( global_check_list[i] )
        {
            sprintf(key, "key %zu", i);
            ret = ghtable_get(ght, key);
            if ( !ret )
                printf(ERROR " %s unreachable", key);

            count++;
        }
    }

    if ( count != ghtable_count(ght) )
        puts(ERROR " Counts missmatch!");
    else
        puts("Count match.");
}

size_t count_holes_in_kl(ghtable* ght)
{
    const key_list_entry* kl = ghtable_key_list(ght);
    size_t ght_count = ghtable_count(ght);
    size_t holes = 0;

    for ( int i = 0; i < ght_count; i++ )
    {
        const char* key = kl[i].key;
        if ( key[0] != 'k')
        {
            printf(RED("x") " dziura at %d \n", i);
            holes++;
        }
    }

    return holes;
}

void fill_half_the_table(ghtable* ght)
{
    for ( size_t i = 0; i < LIMIT/2; i++ )
    {
        size_t key_number = random() % LIMIT;
        sprintf(key, "key %zu", key_number);
        ghtable_set(ght, key, &key_number, sizeof key_number);
    }
}

void fuzzy_test(ghtable* ght)
{
    //range func_range = (range){0, 3};

    for ( size_t i = 0; i < LIMIT; i++ )
    {
        printf("%zu \n", i);
        size_t r =  i % 3; //random_from_range(func_range);
        //printf("%zu selected \n", r);
        switch ( r )
        {
            case 0: get_random(ght); break;
            case 1: del_random(ght); break;
            case 2: random_nth(ght); break;
        }
        set_random(ght);
        if ( i % 1000 == 0 )
        {
            shrink_table(ght);
            if ( ghtable_key_list(ght) && i > LIMIT / 2 )
                ghtable_drop_keylist(ght);
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
