#ifndef GHTABLE_LIB_H
#define GHTABLE_LIB_H

#include <stddef.h>

#define LOAD_FACTOR 0.5

#define ORD 'o'
#define UNORD 'u'

enum ghtable_del_err
{
    TABLE_OR_KEY_NULL = 1,
    ENTRY_NOT_FOUND
};

typedef struct
{
    const void* key;
    size_t key_len;

} key_list_entry;

typedef struct
{
    const void* value;
    size_t size;

} value_view;

typedef struct ghtable ghtable;
typedef struct ghtable_entry ghtable_entry;

typedef struct
{
    const ghtable* ght;
    const ghtable_entry* entry;

} ghtable_iterator;

ghtable* new_ghtable(size_t est_init_count, char type);
void free_ghtable(ghtable* ght);

const key_list_entry* ghtable_key_list(ghtable* ght);

size_t ghtable_capacity(ghtable* ght);
size_t ghtable_count(ghtable* ght);

size_t ghtable_size(ghtable* ght);
size_t ghtable_opt_size(ghtable* ght);
size_t ghtable_key_list_size(ghtable* ght);
size_t ghtable_key_list_opt_size(ghtable* ght);

ghtable* ghtable_grow(ghtable* ght, size_t factor);
ghtable* ghtable_shrink(ghtable* ght);

const void* ghtable_get(ghtable* ght, const char* key);
const void* ghtable_getn(ghtable* ght, const void* key, size_t key_size);
value_view ghtable_view(ghtable* ght, const char* key);
value_view ghtable_viewn(ghtable* ght, const void* key, size_t key_len);

const void* ghtable_set(ghtable* ght, const char* key, void* value, size_t size);
const void* ghtable_setn(ghtable* ght, const void* key, size_t key_size, void* value, size_t value_size);

int ghtable_del(ghtable* ght, const char* key);
int ghtable_deln(ghtable* ght, const void* key, size_t key_len);

const void* ghtable_nth(ghtable* ght, size_t index);
const void* ghtable_nth_key(ghtable* ght, size_t index);
key_list_entry ghtable_nth_kl_entry(ghtable* ght, size_t index);
size_t ghtable_index(ghtable* ght, const char* key);
size_t ghtable_indexn(ghtable* ght, const void* key, size_t key_len);

void ghtable_drop_keylist(ghtable* ght);

ghtable_iterator ghtable_new_iterator(ghtable* ght);
size_t ghtable_iterator_position(ghtable_iterator* it);
ghtable_iterator* ghtable_iterator_seek(ghtable_iterator* it, size_t position);

const void* ghtable_next(ghtable_iterator* it);
const void* ghtable_prev(ghtable_iterator* it);
const void* ghtable_next_key(ghtable_iterator* it);
const void* ghtable_prev_key(ghtable_iterator* it);

void* ghtable_cv(ghtable* ght, const char* key, void* buffer);
void* ghtable_cvn(ghtable* ght, const void* key, size_t key_len, void* buffer);

#endif
