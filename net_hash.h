
#include <stdlib.h>

//  Opaque class structure
typedef struct _nhash nhash_t;

//  @interface
//  Callback function for nhash_foreach method
typedef int (nhash_foreach_fn) (char *key, void *item, void *argument);
//  Callback function for nhash_freefn method
typedef void (nhash_free_fn) (void *data);

//  Create a new, empty hash container
nhash_t *
  nhash_new (void);

//  Destroy a hash container and all items in it
void
  nhash_destroy (nhash_t **self_p);

//  Insert item into hash table with specified key and item.
//  If key is already present returns -1 and leaves existing item unchanged
//  Returns 0 on success.
int
  nhash_insert (nhash_t *self, char *key, void *item);

//  Update item into hash table with specified key and item.
//  If key is already present, destroys old item and inserts new one.
//  Use free_fn method to ensure deallocator is properly called on item.
void
  nhash_update (nhash_t *self, char *key, void *item);

//  Remove an item specified by key from the hash table. If there was no such
//  item, this function does nothing.
void
  nhash_delete (nhash_t *self, char *key);

//  Return the item at the specified key, or null
void *
  nhash_lookup (nhash_t *self, char *key);

//  Reindexes an item from an old key to a new key. If there was no such
//  item, does nothing. Returns 0 if successful, else -1.
int
  nhash_rename (nhash_t *self, char *old_key, char *new_key);

//  Set a free function for the specified hash table item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when hash items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.
void *
  nhash_freefn (nhash_t *self, char *key, nhash_free_fn *free_fn);

//  Return the number of keys/items in the hash table
size_t
  nhash_size (nhash_t *self);

//  Apply function to each item in the hash table. Items are iterated in no
//  defined order.  Stops if callback function returns non-zero and returns
//  final return code from callback function (zero = success).
int
  nhash_foreach (nhash_t *self, nhash_foreach_fn *callback, void *argument);

//  Self test of this class
void
  nhash_test (int verbose);
//  @end