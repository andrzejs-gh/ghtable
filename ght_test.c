#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

void test_key(ghtable* ght, const void* key, size_t key_len)
{
    size_t* value_ptr = ghtable_getn(ght, key, key_len);
    if ( !value_ptr )
        puts("Invalid key.");
    else
        printf("Retrieved value: %zu \n", *value_ptr);
}

void test_random_key_indexes(ghtable* ght, size_t n)
{
    for ( int i = 0; i < n; i++ )
    {
        size_t index = (size_t)(random() % ght->count );
        size_t* value;

        if ( (value = ghtable_get_nth(ght, index) ) )
            printf("Key at index %zu returns value %zu \n", index, *value);
        else
            printf("[ Error ] Index %zu is invalid. \n", index);
    }
}

void shrink_table(ghtable* ght)
{
    printf( "Table size in memory: %zu \n", ght->capacity*sizeof(ghtable_entry) );
    printf( "Key list size in memory: %zu \n",
            ght->key_list_capacity*sizeof(key_list_entry) );

    size_t table_after_shrink = (size_t)(ght->count / LOAD_FACTOR) *
                                            sizeof(ghtable_entry);
    size_t kl_after_shrink = ght->count * sizeof(key_list_entry);


    if (!ghtable_shrink(ght))
    {
        puts("Failure during table shrinking.");
        return;
    }

    printf( "Expected table size after shrinking: %zu \n", table_after_shrink );
    printf( "Table shrunk to: %zu \n", ght->capacity*sizeof(ghtable_entry) );
    if ( table_after_shrink != ght->capacity*sizeof(ghtable_entry) )
        puts("Error, table size after shrinking differs from expected value!");

    printf( "Expected key list size after shrinking: %zu \n", kl_after_shrink );
    printf( "Key list shrunk to: %zu \n",
            ght->key_list_capacity*sizeof(key_list_entry) );
    if ( kl_after_shrink != ght->key_list_capacity*sizeof(key_list_entry) )
        puts("Error, key list size after shrinking differs from expected value!");

}

void delete_keys(ghtable* ght)
{
    size_t rand_keys[5];
    for ( int i = 0; i < 5; i++ )
        rand_keys[i] = random() % ght->count;

    puts("Generated 5 random keys:");
    for ( int i = 0; i < 5; i++ )
    {
        if ( rand_keys[i] % 2 == 0 )
            printf("%zu \n", rand_keys[i]);
        else
            printf("\"key %zu\" \n", rand_keys[i]);
    }
    putchar('\n');

    char key[32];

    puts("Deleting corresponding entries...");
    for ( size_t i = 0; i < 5; i++ )
    {
        if ( rand_keys[i] % 2 == 0 )
        {
            if ( !ghtable_deln(ght, &rand_keys[i], sizeof(size_t) ) )
                printf("Deleted entry for: %zu \n", rand_keys[i]);
            else
                printf("Failed to delete entry for: %zu \n", rand_keys[i]);
        }
        else
        {
            sprintf(key, "key %zu", rand_keys[i]);
            if ( !ghtable_del(ght, key) )
                printf("Deleted entry for: \"%s\" \n", key);
            else
                printf("Failed to delete entry for: \"%s\" \n", key);
        }
    }
    putchar('\n');

    puts("Beginning table shrinking test...");
    shrink_table(ght);
    putchar('\n');

    puts("Trying to retrieve deleted keys:");

    for ( size_t i = 0; i < 5; i++ )
    {
        if ( rand_keys[i] % 2 == 0 )
        {
            if ( !ghtable_getn(ght, &rand_keys[i], sizeof(size_t)) )
                printf("[Ok] table does not return an entry for key: %zu \n", rand_keys[i]);
            else
                printf("Error, table returns an entry for key: %zu \n", rand_keys[i]);
        }
        else
        {
            sprintf(key, "key %zu", rand_keys[i]);
            if ( !ghtable_get(ght, key) )
                printf("[Ok] table does not return an entry for key: \"%s\" \n", key);
            else
                printf("[ Error ] table returns an entry for key: \"%s\" \n", key);
        }
    }

    putchar('\n');
    test_random_keys(ght, 5);
}

void iterate_over_key_list(ghtable* ght)
{
    key_list_entry* keys = ght->keys;
    for ( size_t i = 0; i < ght->count; i++ )
    {
        if ( i % 2 == 0 )
            printf("Index: %zu, key: %zu \n", i, *(size_t*)keys[i].key);
        else
            printf("index: %zu, key: \"%s\" \n", i, (char*)keys[i].key);
    }
}

int main(void)
{
    srandom((unsigned)time(NULL));

    ghtable* ght = new_ghtable(1, ORD);
    size_t n = 1000000;

    printf("Inserting %zu entries... \n", n);
    insert_elements(ght, n);

    test_random_keys(ght, 100);

    printf("\nTesting keys deletion... \n ");
    delete_keys(ght);

    printf("Testing key indexes... \n");
    test_random_key_indexes(ght, 5);

    return 0;
}
