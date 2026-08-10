#ifndef GHTABLE_LIB_H
#define GHTABLE_LIB_H

#include <stddef.h>
#include <stdint.h>

#define LOAD_FACTOR 0.5

#define ORD 'o'
#define UNORD 'u'

#define INVALID_CURSOR SIZE_MAX

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

typedef struct
{
    const void* key;
    const void* value;
    size_t key_len;
    size_t val_size;

} entry_view;

typedef struct ghtable ghtable;
typedef struct ghtable_entry ghtable_entry;

typedef size_t ghtable_cursor;

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
entry_view ghtable_entry_view(ghtable* ght, const char* key);
entry_view ghtable_entry_viewn(ghtable* ght, const void* key, size_t key_len);

const void* ghtable_set(ghtable* ght, const char* key, void* value, size_t size);
const void* ghtable_setn(ghtable* ght, const void* key, size_t key_size, void* value, size_t value_size);

int ghtable_del(ghtable* ght, const char* key);
int ghtable_deln(ghtable* ght, const void* key, size_t key_len);

const void* ghtable_nth(ghtable* ght, size_t index);
const void* ghtable_nth_key(ghtable* ght, size_t index);
key_list_entry ghtable_nth_kl_entry(ghtable* ght, size_t index);
entry_view ghtable_nth_entry_view(ghtable* ght, size_t index);
size_t ghtable_index(ghtable* ght, const char* key);
size_t ghtable_indexn(ghtable* ght, const void* key, size_t key_len);

void ghtable_drop_keylist(ghtable* ght);

ghtable_cursor ghtable_new_cursor(ghtable* ght);
size_t ghtable_cursor_position(ghtable* ght, ghtable_cursor* cursor);
ghtable_cursor ghtable_cursor_seek(ghtable* ght, ghtable_cursor* cursor, size_t position);

const void* ghtable_next(ghtable* ght, ghtable_cursor* cursor);
const void* ghtable_prev(ghtable* ght, ghtable_cursor* cursor);
const void* ghtable_next_key(ghtable* ght, ghtable_cursor* cursor);
const void* ghtable_prev_key(ghtable* ght, ghtable_cursor* cursor);
entry_view ghtable_next_entry_view(ghtable* ght, ghtable_cursor* cursor);
entry_view ghtable_prev_entry_view(ghtable* ght, ghtable_cursor* cursor);

void* ghtable_cv(ghtable* ght, const char* key, void* buffer);
void* ghtable_cvn(ghtable* ght, const void* key, size_t key_len, void* buffer);

#endif
