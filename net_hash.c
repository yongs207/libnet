#ifndef net_hash_h
#define net_hash_h

#include "net_hash.h"
#include <string.h>
#include <assert.h>

#define streq(s1,s2)    (!strcmp ((s1), (s2)))
#define strneq(s1,s2)   (strcmp ((s1), (s2)))
//  Hash table performance parameters

#define INITIAL_SIZE    255     //  Initial size in items
#define LOAD_FACTOR     75      //  Percent loading before splitting
#define GROWTH_FACTOR   200     //  Increase in % after splitting

typedef unsigned int uint;
//  Hash item, used internally only

typedef struct _item_t item_t;
struct _item_t {
  void
    *value;                 //  Opaque item value
  item_t
    *next;                  //  Next item in the hash slot
  unsigned int
    index;                  //  Index of item in table
  char
    *key;                   //  Item's original key
  nhash_free_fn
    *free_fn;               //  Value free function if any
};

//  Hash table structure

struct _nhash {
  size_t
    size;                   //  Current size of hash table
  size_t
    limit;                  //  Current hash table limit
  item_t
    **items;                //  Array of items
  uint
    cached_index;           //  Avoids duplicate hash calculations
};


//  --------------------------------------------------------------------------
//  Local helper function
//  Compute hash for key string

static uint
  s_item_hash (char *key, size_t limit)
{
  uint
    key_hash = 0;

  //  Torek hashing function
  while (*key) {
    key_hash *= 33;
    key_hash += *key;
    key++;
  }
  key_hash %= limit;
  return key_hash;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Lookup item in hash table, returns item or NULL

static item_t *
  s_item_lookup (nhash_t *self, char *key)
{
  item_t *item;
  //  Look in bucket list for item by key
  self->cached_index = s_item_hash (key, self->limit);
  item = self->items [self->cached_index];
  while (item) {
    if (streq (item->key, key))
      break;
    item = item->next;
  }
  return item;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Insert new item into hash table, returns item
//  If item already existed, returns NULL

static item_t *
  s_item_insert (nhash_t *self, char *key, void *value)
{
  //  Check that item does not already exist in hash table
  //  Leaves self->cached_index with calculated hash item
  item_t *item = s_item_lookup (self, key);
  if (item == NULL) {
    item = (item_t *) malloc (sizeof (item_t));
    item->value = value;
    item->key = strdup (key);
    item->index = self->cached_index;
    //  Insert into start of bucket list
    item->next = self->items [self->cached_index];
    self->items [self->cached_index] = item;
    self->size++;
  }
  else
    item = NULL;            //  Signal duplicate insertion
  return item;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Destroy item in hash table, item must exist in table

static void
  s_item_destroy (nhash_t *self, item_t *item, int hard)
{
  //  Find previous item since it's a singly-linked list
  item_t *cur_item = self->items [item->index];
  item_t **prev_item = &(self->items [item->index]);
  while (cur_item) {
    if (cur_item == item)
      break;
    prev_item = &(cur_item->next);
    cur_item = cur_item->next;
  }
  assert (cur_item);
  *prev_item = item->next;
  self->size--;
  if (hard) {
    if (item->free_fn)
      (item->free_fn) (item->value);
    free (item->key);
    free (item);
  }
}


//  --------------------------------------------------------------------------
//  Hash table constructor

nhash_t *
  nhash_new (void)
{
  nhash_t *self = (nhash_t *) malloc (sizeof (nhash_t));
  self->limit = INITIAL_SIZE;
  self->items = (item_t **) malloc (sizeof (item_t *) * self->limit);
  return self;
}


//  --------------------------------------------------------------------------
//  Hash table destructor

void
  nhash_destroy (nhash_t **self_p)
{
  assert (self_p);
  if (*self_p) {
    nhash_t *self = *self_p;
    unsigned int index;
    for (index = 0; index < self->limit; index++) {
      //  Destroy all items in this hash bucket
      item_t *cur_item = self->items [index];
      while (cur_item) {
        item_t *next_item = cur_item->next;
        s_item_destroy (self, cur_item, 1);
        cur_item = next_item;
      }
    }
    if (self->items)
      free (self->items);

    free (self);
    *self_p = NULL;
  }
}


//  --------------------------------------------------------------------------
//  Insert item into hash table with specified key and item
//  If key is already present returns -1 and leaves existing item unchanged
//  Returns 0 on success.

int
  nhash_insert (nhash_t *self, char *key, void *value)
{
  assert (self);
  assert (key);

  //  If we're exceeding the load factor of the hash table,
  //  resize it according to the growth factor
  if (self->size >= self->limit * LOAD_FACTOR / 100) {
    item_t
      *cur_item,
      *next_item;
    item_t
      **new_items;
    size_t
      new_limit;
    unsigned int
      index,
      new_index;

    //  Create new hash table
    new_limit = self->limit * GROWTH_FACTOR / 100;
    new_items = (item_t **) malloc (sizeof (item_t *) * new_limit);

    //  Move all items to the new hash table, rehashing to
    //  take into account new hash table limit
    for (index = 0; index != self->limit; index++) {
      cur_item = self->items [index];
      while (cur_item) {
        next_item = cur_item->next;
        new_index = s_item_hash (cur_item->key, new_limit);
        cur_item->index = new_index;
        cur_item->next = new_items [new_index];
        new_items [new_index] = cur_item;
        cur_item = next_item;
      }
    }
    //  Destroy old hash table
    free (self->items);
    self->items = new_items;
    self->limit = new_limit;
  }
  return s_item_insert (self, key, value)? 0: -1;
}


//  --------------------------------------------------------------------------
//  Update item into hash table with specified key and item.
//  If key is already present, destroys old item and inserts new one.
//  Use free_fn method to ensure deallocator is properly called on item.

void
  nhash_update (nhash_t *self, char *key, void *value)
{
  assert (self);
  assert (key);
  {
    item_t *item = s_item_lookup (self, key);
    if (item) {
      if (item->free_fn)
        (item->free_fn) (item->value);
      item->value = value;
    }
    else
      nhash_insert (self, key, value);
  }

}


//  --------------------------------------------------------------------------
//  Remove an item specified by key from the hash table. If there was no such
//  item, this function does nothing.

void
  nhash_delete (nhash_t *self, char *key)
{
  assert (self);
  assert (key);
  {
    item_t *item = s_item_lookup (self, key);
    if (item)
      s_item_destroy (self, item, 1);
  }
}


//  --------------------------------------------------------------------------
//  Look for item in hash table and return its item, or NULL

void *
  nhash_lookup (nhash_t *self, char *key)
{
  assert (self);
  assert (key);
  {
    item_t *item = s_item_lookup (self, key);
    if (item)
      return item->value;
    else
      return NULL;
  }

}


//  --------------------------------------------------------------------------
//  Reindexes an item from an old key to a new key. If there was no such
//  item, does nothing. If the new key already exists, deletes old item.

int
  nhash_rename (nhash_t *self, char *old_key, char *new_key)
{
  item_t *item = s_item_lookup (self, old_key);
  if (item) {
    item_t *new_item;
    s_item_destroy (self, item, 0);
    new_item = s_item_lookup (self, new_key);
    if (new_item == NULL) {
      free (item->key);
      item->key = strdup (new_key);
      item->index = self->cached_index;
      item->next = self->items [self->cached_index];
      self->items [self->cached_index] = item;
      self->size++;
      return 0;
    }
    else
      return -1;
  }
  else
    return -1;
}


//  --------------------------------------------------------------------------
//  Set a free function for the specified hash table item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when hash items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.

void *
  nhash_freefn (nhash_t *self, char *key, nhash_free_fn *free_fn)
{
  assert (self);
  assert (key);
  {
    item_t *item = s_item_lookup (self, key);
    if (item) {
      item->free_fn = free_fn;
      return item->value;
    }
    else
      return NULL;
  }
  
}


//  --------------------------------------------------------------------------
//  Return size of hash table

size_t
  nhash_size (nhash_t *self)
{
  assert (self);
  return self->size;
}


//  --------------------------------------------------------------------------
//  Apply function to each item in the hash table. Items are iterated in no
//  defined order.  Stops if callback function returns non-zero and returns
//  final return code from callback function (zero = success).

int
  nhash_foreach (nhash_t *self, nhash_foreach_fn *callback, void *argument)
{
  int rc = 0;
  uint index;
  assert (self);

  for (index = 0; index != self->limit; index++) {
    item_t *item = self->items [index];
    while (item) {
      //  Invoke callback, passing item properties and argument
      item_t *next = item->next;
      rc = callback (item->key, item->value, argument);
      if (rc)
        break;          //  End if non-zero return code
      item = next;
    }
  }
  return rc;
}

#endif