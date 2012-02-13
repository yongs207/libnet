//  Opaque class structure

#include <stdlib.h>

typedef struct _nlist nlist_t;

//  @interface
//  Create a new list container
nlist_t * nlist_new (void);

//  Destroy a list container
void nlist_destroy (nlist_t **self_p);

//  Return first item in the list, or null
void * nlist_first (nlist_t *self);

//  Return next item in the list, or null
void *nlist_next (nlist_t *self);

//  Append an item to the end of the list
void nlist_append (nlist_t *self, void *item);

//  Push an item to the start of the list
void  nlist_push (nlist_t *self, void *item);

//  Pop the item off the start of the list, if any
void *  nlist_pop (nlist_t *self);

//  Remove the specified item from the list if present
void  nlist_remove (nlist_t *self, void *item);

//  Copy the entire list, return the copy
nlist_t *  nlist_copy (nlist_t *self);

//  Return number of items in the list
size_t  nlist_size (nlist_t *self);


//  @end
