# GHTABLE Documentation

## Table of contents

- [Description](#description)
- [Minimal usage example](#minimal-usage-example)
- [Full method list](#full-method-list)

---

## Description

Flexible universal generic hash table implemented in C with zero external dependencies. 

General overview: 
- optional ordering (`ghtable` is initialized as either **ordered** or **unordered**, and the ordering can be droped at any time going from  **ordered** -> **unordered**)
- arbitrary keys
- arbitrary data
- open addressing with linear probing
- no tombstones, entries are always shifted to fill the gaps
- hashing via **FNV-1a**
- `LOAD_FACTOR` tweakable in the `ghtable.h`
- dynamicaly growing 
- minimizable to the optimal size defined by the `LOAD_FACTOR`

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>

---

## Minimal usage example

```c
ghtable* ght = new_ghtable(10, ORD); // reserved count = 10, ordered

int some_value = 33;
typedef struct
{
    int val_1;
    size_t val_2;
} some_t;

some_t val = {-22, 44};

/* example table:
    *
    [0th entry] "key0" : 33,
    [1st entry] "key1" : 0.1234,
    [2nd entry] val : 33,
    [3rd entry] 44 : val
*/

// using string keys
ghtable_set(ght, "key0", &some_value, sizeof some_value);    // "key0" : 33
ghtable_set(ght, "key1", &(double){0.1234}, sizeof(double)); // "key1" : 0.1234

// using binary keys
ghtable_setn(ght, &val, sizeof val, &some_value, sizeof some_value); // val : 33
ghtable_setn(ght, &(int){44}, sizeof(int), &val, sizeof val);        // 44 : val

// =============================================================================

// getting values for "key0" and "key1"
const int* int_value = ghtable_get(ght, "key0");         // 33
const double* db_val = ghtable_get(ght, "key1");         // 0.1234

// getting values for the byte keys
int_value =       ghtable_getn(ght, &val, sizeof val);         // 33
const some_t* p = ghtable_getn(ght, &(int){44}, sizeof(int));  // val

// getting 1st value
db_val = ghtable_nth(ght, 1); // 0.1234

// getting 1st key
const char* key_ptr = ghtable_nth_key(ght, 1); // "key1"

// getting index of "key1" and val
size_t key_index = ghtable_index(ght, "key1");            // 1
size_t val_index = ghtable_indexn(ght, &val, sizeof val); // 2

ghtable_drop_keylist(ght);         // drops the ordering
key_ptr = ghtable_nth_key(ght, 1); // now returns NULL

// freeing ghtable
free_ghtable(ght);
```

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>

---

## Full method list

### Creation and destruction

- [new_ghtable](#-new_ghtable-)
- [free_ghtable](#-free_ghtable-)

### Attributes

- [ghtable_count](#-ghtable_count-)
- [ghtable_capacity](#-ghtable_capacity-)
- [ghtable_size](#-ghtable_size-)
- [ghtable_opt_size](#-ghtable_opt_size-)
- [ghtable_key_list](#-ghtable_key_list-)
- [ghtable_key_list_size](#-ghtable_key_list_size-)
- [ghtable_key_list_opt_size](#-ghtable_key_list_opt_size-)

### Getting values, keys and indexes

- [ghtable_get](#-ghtable_get-)
- [ghtable_getn](#-ghtable_getn-)
- [ghtable_view](#-ghtable_view-)
- [ghtable_viewn](#-ghtable_viewn-)
- [ghtable_entry_view](#-ghtable_entry_view-)
- [ghtable_entry_viewn](#-ghtable_entry_viewn-)
- [ghtable_nth](#-ghtable_nth-)
- [ghtable_nth_key](#-ghtable_nth_key-)
- [ghtable_nth_kl_entry](#-ghtable_nth_kl_entry-)
- [ghtable_index](#-ghtable_index-)
- [ghtable_indexn](#-ghtable_indexn-)
- [ghtable_cv](#-ghtable_cv-)
- [ghtable_cvn](#-ghtable_cvn-)

### Inserting and deleting entries

- [ghtable_set](#-ghtable_set-)
- [ghtable_setn](#-ghtable_setn-)
- [ghtable_del](#-ghtable_del-)
- [ghtable_deln](#-ghtable_deln-)

### Iteration

- [ghtable_new_cursor](#-ghtable_new_cursor-)
- [ghtable_cursor_position](#-ghtable_cursor_position-)
- [ghtable_cursor_seek](#-ghtable_cursor_seek-)
- [ghtable_next](#-ghtable_next-)
- [ghtable_prev](#-ghtable_prev-)
- [ghtable_next_key](#-ghtable_next_key-)
- [ghtable_prev_key](#-ghtable_prev_key-)
- [ghtable_next_entry_view](#-ghtable_next_entry_view-)
- [ghtable_prev_entry_view](#-ghtable_prev_entry_view-)

### Manual resizing and dropping key list

- [ghtable_grow](#-ghtable_grow-)
- [ghtable_shrink](#-ghtable_shrink-)
- [ghtable_drop_keylist](#-ghtable_drop_keylist-)


<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>

---

## Creation and destruction

### ** **new_ghtable** **

```c
ghtable* new_ghtable(size_t est_init_count, char type);
```

Creates new `ghtable` and returns pointer to it. In case of a failure, it return `NULL`.
The estimated initial count value `est_init_count` must be non-zero and `type` must be either `ORD` or `UNORD`:
```c
// ghtable.h

#define ORD 'o'     // ordered
#define UNORD 'u'   // unordered
```
Initial hash table capacity is set to `2 * est_init_count`, and (for an ordered `ghtable`), key list reserves `est_init_count` slots.

### Arguments:
- `size_t est_init_count` ( must be > 0 )
- `char type` ( must be `ORD` or `UNORD` )

### Returns:
- **success**: `ghtable*` ptr to ghtable instance
- **failure**: `NULL`
    - `est_init_count == 0`
    - `type` is neither `ORD` nor `UNORD`
    - allocation failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **free_ghtable** **

```c
void free_ghtable(ghtable* ght);
```

Frees `ghtable` and all its resources.

### Arguments:
- `ghtable* ght` 

### Returns:
`void`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Attributes

### ** **ghtable_count** **

```c
size_t ghtable_count(ghtable* ght);
```

Returns hash table element count.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: non-zero element count
- **failure**: `0`
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_capacity** **

```c
size_t ghtable_capacity(ghtable* ght);
```

Returns hash table capacity.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: non-zero capacity
- **failure**: 0 
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_size** **

```c
size_t ghtable_size(ghtable* ght);
```

Returns hash table size in memory (bytes).

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: non-zero size
-  **failure** 0 
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_opt_size** **

```c
size_t ghtable_opt_size(ghtable* ght);
```

Calculates and returns hash table optimal size in bytes which is defined as: 
```c
// ghtable.c

size_t opt_size = (size_t)(ght->count / LOAD_FACTOR) * sizeof(ghtable_entry);
if ( opt_size < MINIMAL_SIZE )
    opt_size = MINIMAL_SIZE;
```
`MINIMAL_SIZE` is defined as `2 * sizeof(ghtable_entry)`. 
This is the size that the hash table would shrink to if [ghtable_shrink](#-ghtable_shrink-) was called. 
If hash table count is 0, the function returns `MINIMAL_SIZE` without calculation.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: non-zero size
- **failure**: 0 
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_key_list** **

```c
const key_list_entry* ghtable_key_list(ghtable* ght);
```

Every **ordered** `ghtable` holds its key list: `key_list_entry* keys` with `key_list_entry` defined as:
```c
// ghtable.h

typedef struct
{
    const void* key;
    size_t key_len;

} key_list_entry;
```
The function returns pointer `key_list_entry*` to the first entry.
If `ghtable` is unordered, the function returns `NULL`.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: `const key_list_entry*` pointer to the first key list entry
- **failure**: `NULL` 
    - `ghtable` is not ordered
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_key_list_size** **

```c
size_t ghtable_key_list_size(ghtable* ght);
```

Returns size in memory (bytes) currently reserved by the key list. If `ghtable` is not ordered, the function returns 0. 

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: non-zero size
- **failure**: 0 
    - `ghtable` is not ordered
    - `ght == NULL` 

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_key_list_opt_size** **

```c
size_t ghtable_key_list_opt_size(ghtable* ght);
```

Returns the optimal key list size (bytes) defined as:
```c
// ghtable.c

ght->count * sizeof(key_list_entry);
```
It's the size the key list would shrink to if [ghtable_shrink](#-ghtable_shrink-) was called (and if `ghtable` is ordered, otherwise `ght->keys == NULL` as the key list isn't even allocated).
The value is calculated and returned even if `ghtable` is not ordered.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: non-zero size
- **failure**: 0 
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### Getting values, keys and indexes

### ** **ghtable_get** **

```c
const void* ghtable_get(ghtable* ght, const char* key);
```

Takes a string key and returns pointer to a value. If entry is not found in the hash table, the function returns `NULL`.

### Arguments:
- `ghtable* ght` 
- `const char* key*`

### Returns:
- **success**: `const void*` pointer to the value
- **failure**: `NULL`
    - entry not found
    - `ght == NULL`
    - `key == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_getn** **

```c
const void* ghtable_getn(ghtable* ght, const void* key, size_t key_size);
```

Same as [ghtable_get](#-ghtable_get-) but takes a byte key instead of a string key and returns pointer to a value. If entry is not found in the hash table, the function returns `NULL`.

### Arguments:
- `ghtable* ght` 
- `const void* key` 
- `size_t key_size`

### Returns:
- **success**: `const void*` pointer to the value
- **failure**: `NULL`
    - entry not found
    - `ght == NULL`
    - `key == NULL`
    - `key_size == 0`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_view** **

```c
value_view ghtable_view(ghtable* ght, const char* key);
```

Takes a string key and returns `value_view` struct, which is defined as:
```c
// ghtable.h

typedef struct
{
    const void* value;  // pointer to a value
    size_t size;        // value size

} value_view;
```
If entry is not found, the function returns:
```c
(value_view){NULL, 0};
```

### Arguments:
- `ghtable* ght` 
- `const char* key`

### Returns:
- **success**: `value_view` view struct
- **failure**: `value_view` empty view struct `(value_view){NULL, 0}`
    - entry not found
    - empty `key`
    - `ght == NULL`
    - `key == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_viewn** **

```c
value_view ghtable_viewn(ghtable* ght, const void* key, size_t key_len);
```

Same as [ghtable_view](#-ghtable_view-) but takes a byte key instead of a string key and returns `value_view` struct, which is defined as:
```c
// ghtable.h

typedef struct
{
    const void* value;  // pointer to a value
    size_t size;        // value size

} value_view;
```
If entry is not found, the function returns:
```c
(value_view){NULL, 0};
```

### Arguments:
- `ghtable* ght` 
- `const void* key`
- `size_t key_len`

### Returns:
- **success**: `value_view` view struct
- **failure**: `value_view` empty view struct `(value_view){NULL, 0}`
    - entry not found
    - `ght == NULL`
    - `key == NULL`
    - `key_len == 0`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_entry_view** **

```c
entry_view ghtable_entry_view(ghtable* ght, const char* key);
```

Takes a string key and returns `entry_view` struct, which is defined as:
```c
// ghtable.h

typedef struct
{
    const void* key;
    const void* value;
    size_t key_len;
    size_t val_size;

} entry_view;
```
If entry is not found, the function returns:
```c
(entry_view){NULL, NULL, 0, 0};
```

### Arguments:
- `ghtable* ght` 
- `const char* key`

### Returns:
- **success**: `entry_view` struct
- **failure**: `entry_view` empty struct `(entry_view){NULL, NULL, 0, 0}`
    - entry not found
    - empty `key`
    - `ght == NULL`
    - `key == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_entry_viewn** **

```c
entry_view ghtable_entry_viewn(ghtable* ght, const void* key, size_t key_len);
```

Same as [ghtable_entry_view](#-ghtable_view-) but takes a byte key instead of a string key and returns `entry_view` struct, which is defined as:
```c
// ghtable.h

typedef struct
{
    const void* key;
    const void* value;
    size_t key_len;
    size_t val_size;

} entry_view;
```
If entry is not found, the function returns:
```c
(entry_view){NULL, NULL, 0, 0};
```

### Arguments:
- `ghtable* ght` 
- `const void* key`
- `size_t key_len`

### Returns:
- **success**: `entry_view` view struct
- **failure**: `entry_view` empty view struct `(entry_view){NULL, NULL, 0, 0}`
    - entry not found
    - `ght == NULL`
    - `key == NULL`
    - `key_len == 0`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_nth** **

```c
const void* ghtable_nth(ghtable* ght, size_t index);
```

Returns nth entry's value. If `ghtable` is unordered or the index is invalid, the function returns `NULL`.

### Arguments:
- `ghtable* ght` 
- `size_t index`

### Returns:
- **success**: `const void*` pointer to the value
- **failure**: `NULL`
    - invalid index
    - `ghtable` is not ordered
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_nth_key** **

```c
const void* ghtable_nth_key(ghtable* ght, size_t index);
```

Returns nth entry's key. If `ghtable` is unordered or the index is invalid, the function returns `NULL`.

### Arguments:
- `ghtable* ght` 
- `size_t index`

### Returns:
- **success**: `const void*` pointer to the nth key
- **failure**: `NULL`
    - invalid index
    - `ghtable` is not ordered
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_nth_kl_entry** **

```c
key_list_entry ghtable_nth_kl_entry(ghtable* ght, size_t index);
```

Returns nth entry from the key list (corresponding to the nth entry in the hash table):
```c
// ghtable.h

typedef struct
{
    const void* key;
    size_t key_len;

} key_list_entry;
```
If the `ghtable` is unordered, the function returns:
```c
(key_list_entry){NULL, 0};
```

### Arguments:
- `ghtable* ght` 
- `size_t index`

### Returns:
- **success**: `key_list_entry` nth key list entry
- **failure**: `key_list_entry` empty entry `(key_list_entry){NULL, 0}`
    - invalid index
    - `ghtable` is unordered
    - `ght == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_index** **

```c
size_t ghtable_index(ghtable* ght, const char* key);
```

Takes a string key and returns its index. If the key is not found or `ghtable` is unordered, the function returns `SIZE_MAX`.

### Arguments:
- `ghtable* ght` 
- `const char* key`

### Returns:
- **success**: `size_t` key index
- **failure**: `SIZE_MAX`
    - key not found
    - `key` is empty
    - `ghtable` is not ordered
    - `ght == NULL`
    - `key == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_indexn** **

```c
size_t ghtable_indexn(ghtable* ght, const void* key, size_t key_len);
```

Same as [ghtable_index](#-ghtable_index-) but takes a byte key instad of a string key and returns its index. If the key is not found or `ghtable` is unordered, the function returns `SIZE_MAX`.

### Arguments:
- `ghtable* ght` 
- `const void* key`
- `size_t key_len`

### Returns:
- **success**: `size_t` key index
- **failure**: `size_t` `SIZE_MAX`
    - key not found
    - `ghtable` is not ordered
    - `ght == NULL`
    - `key == NULL`
    - `key_len == 0`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_cv** **

```c
void* ghtable_cv(ghtable* ght, const char* key, void* buffer);
```

Copies the value for a given key to the provided buffer. Returns back the pointer to the buffer on success, and `NULL` on failure.

### Arguments:
- `ghtable* ght` 
- `const char* key`
- `void* buffer`

### Returns:
- **success**: `void*` pointer to the buffer
- **failure**: `NULL`
    - entry not found
    - `key` is empty
    - `ght == NULL`
    - `key == NULL`
    - `buffer == NULL`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_cvn** **

```c
void* ghtable_cvn(ghtable* ght, const void* key, size_t key_len, void* buffer);
```

Same as [ghtable_cv](#-ghtable_cv-) but takes a byte key instead of a string key. Copies the value for a given key to the provided buffer. Returns back the pointer to the buffer on success, and `NULL` on failure.

### Arguments:
- `ghtable* ght` 
- `const void* key`
- `size_t key_len`
- `void* buffer`

### Returns:
- **success**: `void*` pointer to the buffer
- **failure**: `NULL`
    - entry not found
    - `ght == NULL`
    - `key == NULL`
    - `buffer == NULL`
    - `key_len == 0`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### Inserting and deleting entries

### ** **ghtable_set** **

```c
const void* ghtable_set(ghtable* ght, const char* key, void* value, size_t size);
```

Inserts new entry to the hash table, or **overwrites** existing one. Requires pointer to the value and its size, so the buffer can be copied.

### Arguments:
- `ghtable* ght` 
- `const char* key`
- `void* value`
- `size_t size`

### Returns:
- **success**: pointer to the allocated value
- **failure**: `NULL`
    - `key == NULL`
    - `key` is empty 
    - `value == NULL` 
    - `size == 0`
    - `ght == NULL`
    - allocation failure
    - failed hash table resize

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_setn** **

```c
const void* ghtable_setn(ghtable* ght, const void* key, size_t key_size, void* value, size_t value_size);
```

Same as [ghtable_set](#-ghtable_set-) but takes a byte key instead of a string key. Inserts new entry to the hash table, or **overwrites** existing one. Requires size of the key, pointer to the value, and size of the value.

### Arguments:
- `ghtable* ght` 
- `const void* key`
- `size_t key_size`
- `void* value`
- `size_t value_size`

### Returns:
- **success**: pointer to the allocated value
- **failure**: `NULL`
    - `ght == NULL`
    - `key == NULL`
    - `key` is empty 
    - `key_size == 0`
    - `value == NULL` 
    - `value_size == 0`
    - allocation failure
    - failed hash table resize

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_del** **

```c
int ghtable_del(ghtable* ght, const char* key);
```

Deletes an entry from the hash table. Removes the coresponding `key_list_entry` from the key list if `ghtable` is ordered, shifting following keys down.

### Arguments:
- `ghtable* ght` 
- `const char* key`

### Returns:
- **success**: 0
- **failure**: an error code:
    - `TABLE_OR_KEY_NULL` ( 1 ) 
    - `ENTRY_NOT_FOUND` ( 2 )

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_deln** **

```c
int ghtable_deln(ghtable* ght, const void* key, size_t key_len);
```

Same as [ghtable_del](#-ghtable_del-), but takes a byte key instead of a string key. Deletes an entry from the hash table. Removes the coresponding `key_list_entry` from the key list if `ghtable` is ordered, shifting following keys down. 

### Arguments:
- `ghtable* ght` 
- `const void* key`
- `size_t key_len`

### Returns:
- **success**: 0
- **failure**: an error code:
    - `TABLE_OR_KEY_NULL` ( 1 ) 
    - `ENTRY_NOT_FOUND` ( 2 )

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

## Iteration

### ** **ghtable_new_cursor** **

```c
ghtable_cursor ghtable_new_cursor(ghtable* ght);
```

The hash table can be iterated over (both forward and in reverse, see methods below) using `ghtable_cursor`. The function returns new cursor pointing to the first element. If the hash table is empty, the function returns `INVALID_CURSOR`. 
A valid cursor can be pointed to arbitrary position (see ghtable_cursor_seek), but once the cursor is moved beyond hash table bounds in either direction, it becomes exhausted and `INVALID_CURSOR`.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: `ghtable_cursor` pointing to the first entry
- **failure**: `INVALID_CURSOR`
    - `ght == NULL`
    - hash table is empty

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_cursor_position** **

```c
size_t ghtable_cursor_position(ghtable* ght, ghtable_cursor* cursor);
```

Returns cursor position. If the cursor value is `INVALID_CURSOR`, the function returns `SIZE_MAX`.

### Arguments:
- `ghtable* ght` 
- `ghtable_cursor* cursor`

### Returns:
- **success**: `size_t` cursor position
- **failure**: `SIZE_MAX`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_cursor_seek** **

```c
ghtable_cursor ghtable_cursor_seek(ghtable* ght, ghtable_cursor* cursor, size_t position);
```

Moves the cursor to specified position. Returns modified cursor by value, on failure the cursor becomes `INVALID_CURSOR`. 

### Arguments:
- `ghtable* ght` 
- `ghtable_cursor* cursor`
- `size_t position`

### Returns:
- **success**: `ghtable_cursor` modified cursor
- **failure**: `INVALID_CURSOR`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`
    - `position` is out of bounds (>= hash table count)

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_next** **

```c
const void* ghtable_next(ghtable* ght, ghtable_cursor* cursor);
```

Takes the cursor and returns ptr to the **next** value. Before returning, the cursor is moved one position forward. If the cursor is exhausted/invalid, the function returns `NULL`.

### Arguments:
- `ghtable* ght` 
- `ghtable* cursor`

### Returns:
- **success**: `const void*` pointer to the **next** value
- **failure**: `NULL`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_prev** **

```c
const void* ghtable_prev(ghtable* ght, ghtable_cursor* cursor);
```

Takes the cursor and returns ptr to the **previous** value. Before returning, the cursor is moved one position backward. If the cursor is exhausted/invalid, the function returns `NULL`.

### Arguments:
- `ghtable* ght`
- `ghtable* cursor`

### Returns:
- **success**: `const void*` pointer to the **previous** value
- **failure**: `NULL`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_next_key** **

```c
const void* ghtable_next_key(ghtable* ght, ghtable_cursor* cursor);
```

Takes the cursor and returns ptr to the **next** key. Before returning, the cursor is moved one position forward. If the cursor is exhausted/invalid, the function returns `NULL`.

### Arguments:
- `ghtable* ght`
- `ghtable* cursor`

### Returns:
- **success**: `const void*` pointer to the **next** key
- **failure**: `NULL`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_prev_key** **

```c
const void* ghtable_prev_key(ghtable* ght, ghtable_cursor* cursor);
```

Takes the cursor and returns ptr to the **prev** key. Before returning, the cursor is moved one position backward. If the cursor is exhausted/invalid, the function returns `NULL`.

### Arguments:
- `ghtable* ght`
- `ghtable* cursor`

### Returns:
- **success**: `const void*` pointer to the **prev** key
- **failure**: `NULL`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_next_entry_view** **

```c
entry_view ghtable_next_entry_view(ghtable* ght, ghtable_cursor* cursor);
```

Takes the cursor and returns `entry_view` struct for the **next** entry:
```c
// ghtable.h

typedef struct
{
    const void* key;
    const void* value;
    size_t key_len;
    size_t val_size;

} entry_view;
```
On failure the function returns `(entry_view){NULL, NULL, 0, 0}`.

### Arguments:
- `ghtable* ght` 
- `ghtable_cursor* cursor`

### Returns:
- **success**: `entry_view` of the **next** entry
- **failure**: `(entry_view){NULL, NULL, 0, 0}`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_prev_entry_view** **

```c
entry_view ghtable_prev_entry_view(ghtable* ght, ghtable_cursor* cursor);
```

Takes the cursor and returns `entry_view` struct for the **previous** entry:
```c
// ghtable.h

typedef struct
{
    const void* key;
    const void* value;
    size_t key_len;
    size_t val_size;

} entry_view;
```
On failure the function returns `(entry_view){NULL, NULL, 0, 0}`.

### Arguments:
- `ghtable* ght` 
- `ghtable_cursor* cursor`

### Returns:
- **success**: `entry_view` of the **previous** entry
- **failure**: `(entry_view){NULL, NULL, 0, 0}`
    - `ght == NULL`
    - `cursor == NULL`
    - `*cursor == INVALID_CURSOR`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### Manual resizing and dropping key list

### ** **ghtable_grow** **

```c
ghtable* ghtable_grow(ghtable* ght, size_t factor);
```

Grows hash table capacity by a specified factor. Causes hash table rebuild each time. Returns back pointer to the `ghtable` on success and `NULL` on failure.

### Arguments:
- `ghtable* ght` 
- `size_t factor`

### Returns:
- **success**: `ghtable* ght`
- **failure**: `NULL`
    - `factor < 2`
    - `ght == NULL`
    - allocation failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_shrink** **

```c
ghtable* ghtable_shrink(ghtable* ght);
```

Shrinks both the hash table and the key list (if `ghtable` is **ordered**) to the optimal size ( see [ghtable_opt_size](#-ghtable_opt_size-), [ghtable_key_list_opt_size](#-ghtable_key_list_opt_size-) ).
Can not shrink an empty hash table, returns `NULL` if hash table count is 0.

### Arguments:
- `ghtable* ght` 

### Returns:
- **success**: `ghtable* ght`
- **failure**: `NULL`
    - hash table count is 0
    - `ght == NULL`
    - allocation failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **ghtable_drop_keylist** **

```c
void ghtable_drop_keylist(ghtable* ght);
```

Frees the key list if `ghtable` is **ordered**, changing `ghtable` from **ordered** -> **unordered**. Applied on **unordered** `ghtable` just returns without doing anything.

### Arguments:
- `ghtable* ght` 

### Returns:
- `void`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---
