#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ghtable.h"
#include "time_exec.h"

void insert_elements(ghtable* ght, size_t n)
{
    bool is_even = true;
    char buffer[256];

    for ( size_t i = 0; i < n; i++ )
    {
        if ( is_even )
        {
            if ( !ghtable_setn(ght, &i, sizeof i, &i, sizeof i) )
                printf("Failed insertion at %zu\n", i);

            is_even = false;
        }
        else
        {
            sprintf(buffer, "key %zu", i);
            if ( !ghtable_set(ght, buffer, &i, sizeof i) )
                printf("Failed insertion at %zu\n", i);

            is_even = true;
        }
    }
}

void test_random_keys(ghtable* ght, size_t n)
{
    size_t count = ght->count;
    char key[32];
    size_t i;
    double t;
    size_t* value_ptr;

    printf("Testing %zu random keys: \n", n);

    while ( n-- )
    {
        i = (size_t)(random() % count);

        if ( i % 2 == 0)
        {
            TIME_STORE(
                t,
                value_ptr = ghtable_getn(ght, &i, sizeof i);
            );
            if ( value_ptr )
                printf("Retrieved: %zu : %zu in t = %.9f s \n", i, *value_ptr, t);
            else
                printf("Failed to retrieve value for %zu \n", i);
        }
        else
        {
            sprintf(key, "key %zu", i);

            TIME_STORE(
                t,
                value_ptr = ghtable_get(ght, key);
            );
            if ( value_ptr )
                printf("Retrieved: \"%s\" : %zu in t = %.9f s \n", key, *value_ptr, t);
            else
                printf("Failed to retrieve value for key: \"%s\" \n", key);
        }
    }
}

void delete_keys_and_shrink(ghtable* ght)
{
    size_t rand_keys[5];
    for ( int i = 0; i < 5; i++ )
        rand_keys[i] = random() % ght->count;

    puts("Generated 5 random keys:\n");
    for ( int i = 0; i < 5; i++ )
        printf("key %zu \n", rand_keys[i]);
    putchar('\n');

    char key[32];

    for ( size_t i = 0; i < 5; i++ )
    {
        if ( i % 2 == 0 )
        {
            if ( !ghtable_deln(ght, &i, sizeof i) )
                printf("Deleted key %zu \n", rand_keys[i]);
            else
                printf("Failed to delete key %zu \n", rand_keys[i]);
        }
        else
        {
            sprintf(key, "key %zu", i);
            if ( !ghtable_del(ght, key) )
                printf("Deleted key %zu \n", rand_keys[i]);
            else
                printf("Failed to delete key %zu \n", rand_keys[i]);
        }
    }

    printf( "Table size in memory: %zu \n", ght->capacity*sizeof(ghtable_entry) );
    printf( "Key list size in memory: %zu \n",
            ght->key_list_capacity*sizeof(key_list_entry) );
    if (!ghtable_shrink(ght))
    {
        puts("Failure during table shrinking.");
        return;
    }
    printf( "Table shrunk to: %zu \n", ght->capacity*sizeof(ghtable_entry) );
    printf( "Key list shrunk to: %zu \n",
            ght->key_list_capacity*sizeof(key_list_entry) );

    puts("Trying to retrieve deleted keys:");

    for ( size_t i = 0; i < 5; i++ )
    {
        if ( i % 2 == 0 )
        {
            if ( !ghtable_getn(ght, &i, sizeof i) )
                printf("[Ok] table does not return an entry for key: %zu", i);
            else
                printf("Error, table returns an entry for key: %zu", i);
        }
        else
        {
            sprintf(key, "key %zu", i);
            if ( !ghtable_del(ght, key) )
                printf("[Ok] table does not return an entry for key: \"%s\"", key);
            else
                printf("Error, table returns an entry for key: \"%s\"", key);
        }
    }

    // testing 5 random keys
    test_random_keys(ght, 5);
}

int main(void)
{
    ghtable* ght = new_ghtable(1, ORD);
    insert_elements(ght, 1000000);

    test_random_keys(ght, 100);
    delete_keys_and_shrink(ght);

    return 0;
}
