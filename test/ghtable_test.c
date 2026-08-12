#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "../ghtable.h"
#include "time_exec.h"

#define LIMIT 1000000

#define RED(x) "\033[31m" x "\033[0m"
#define GREEN(x) "\033[32m" x "\033[0m"

#define OK "[ " GREEN("OK") " ]"
#define ERROR "[ " RED("ERROR") " ]"

#define SAVE_POS "\033[s"
#define RESTORE_POS "\033[u"
#define BOTTOM_LEFT "\033[999;1H"

char key[32];

size_t i = 0;
bool global_check_list[LIMIT] = {false};

static inline bool is_entry_valid(const char* str_key, size_t value)
{
    size_t val_from_key = (size_t)( strtoull(&str_key[4], NULL, 10) );
    if ( val_from_key != value )
        return false;

    return true;
}

static inline size_t random_from_range(size_t min, size_t max)
{
    return min + random() % (max - min);
}

static inline void draw_progress(void)
{
    printf(SAVE_POS);
    printf(BOTTOM_LEFT);
    printf("[ COMPLETED ] [ %.2f %% ]", (double)i / LIMIT * 100);
    printf(RESTORE_POS);
    fflush(stdout);
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
    {
        printf(OK " Set: \"%s\" : %zu in t = %.9f s \n", key, key_number, t);
        draw_progress();
    }
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
    size_t key_len = sprintf(key, "key %zu", key_number);

    TIME_STORE( t,
                value_ptr = ghtable_getn(ght, key, key_len);
    );
    if ( value_ptr )
    {
        if ( *value_ptr == key_number )
        {
            printf(OK " Retrieved: \"%s\" : %zu in t = %.9f s \n",
                   key, key_number, t);
            draw_progress();

            if ( ghtable_key_list(ght) &&
                 ghtable_nth(ght, ghtable_index(ght, key)) != value_ptr )
            {
                printf(ERROR " nth index does not match the value!");
                assert(false);
            }
        }
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
    }

}

void del_random(ghtable* ght)
{
    int ret;
    double t;

    size_t key_number = random() % LIMIT;
    size_t key_len = sprintf(key, "key %zu", key_number);

    TIME_STORE( t,
                ret = ghtable_deln(ght, key, key_len);
    );
    if ( !ret )
    {
        printf(OK " Deleted: \"%s\" : %zu in t = %.9f s \n", key, key_number, t);
        draw_progress();
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
        assert(false);
    }
    else if ( !is_entry_valid(kl_entry.key, *val) )
    {
        printf(ERROR " Invalid entry: \"%s\" : " RED("%zu") "\n",
               (char*)kl_entry.key, *val);
        assert(false);
    }

    printf(OK " Entry number %zu verified \n", n);
    draw_progress();
}

void shrink_table(ghtable* ght)
{
    size_t opt_size = ghtable_opt_size(ght);
    size_t key_list_opt_size = ghtable_key_list_opt_size(ght);
    bool failure = false;

    void* ret = ghtable_shrink(ght);
    if ( !ret && ghtable_count(ght) )
    {
        puts(ERROR " ghtable_shrink failure. Could be a realloc failure.");
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

    draw_progress();
}

void iterator_test(ghtable* ght)
{
    size_t ght_count = ghtable_count(ght);
    if ( !ght_count )
        return;

    ghtable_cursor cursor = ghtable_new_cursor(ght);

    size_t r = random_from_range( 0, ght_count );

    if ( ghtable_cursor_seek(ght, &cursor, r) == INVALID_CURSOR )
    {
        puts(ERROR " Cursor seek failure!");
        assert(false);
    }

    const size_t* val = ghtable_next(ght, &cursor);
    if ( !val )
    {
        puts(ERROR " Invalid cursor.");
        assert(false);
    }
    if ( cursor == INVALID_CURSOR )
        cursor = ghtable_new_cursor(ght);

    size_t cursor_position = ghtable_cursor_position(ght, &cursor);
    size_t entry_count = ght_count - cursor_position;

    while ( entry_count-- )
    {
        if ( !ghtable_next(ght, &cursor) )
        {
            puts(ERROR " Cursor moved out of bounds");
            assert(false);
        }
    }

    if ( cursor != INVALID_CURSOR )
    {
        puts(ERROR " Cursor has not been exhausted!");
        assert(false);
    }
    puts(OK " Iterator verified.");
}

void rand_test(ghtable* ght)
{
    for ( i = 0; i < LIMIT; i++ )
    {
        size_t r =  i % 3; //random_from_range(func_range);
        switch ( r )
        {
            case 0: get_random(ght); break;
            case 1: del_random(ght); break;
            case 2: random_nth(ght); break;
        }
        set_random(ght);


        if ( i % 10000 == 0 )
        {
            shrink_table(ght);
            iterator_test(ght);

            if ( ghtable_key_list(ght) && i > LIMIT / 2 )
                ghtable_drop_keylist(ght);
        }
    }
}

int main(void)
{
    srandom((unsigned)time(NULL));

    ghtable* ght = new_ghtable(1, ORD);
    rand_test(ght);

    free_ghtable(ght);

    return 0;
}
