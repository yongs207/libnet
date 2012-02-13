#ifndef net_list_h
#define net_list_h

#include <assert.h>
#include "net_list.h"





struct node_t {
  struct node_t
    *next;
  void
    *item;
};

//  Actual list object

struct _nlist {
  struct node_t
    *head, *tail;
  struct node_t
    *cursor;
  size_t
    size;
};


//  --------------------------------------------------------------------------
//  List constructor

nlist_t *
  nlist_new (void)
{
  nlist_t *self = (nlist_t *) malloc (sizeof (nlist_t));
  return self;
}


//  --------------------------------------------------------------------------
//  List destructor

void
  nlist_destroy (nlist_t **self_p)
{
  assert (self_p);
  if (*self_p) {
    nlist_t *self = *self_p;
    struct node_t *node, *next;
    for (node = (*self_p)->head; node != NULL; node = next) {
      next = node->next;
      free (node);
    }
    free (self);
    *self_p = NULL;
  }
}


//  --------------------------------------------------------------------------
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.

void *
  nlist_first (nlist_t *self)
{
  assert (self);
  self->cursor = self->head;
  if (self->cursor)
    return self->cursor->item;
  else
    return NULL;
}


//  --------------------------------------------------------------------------
//  Return the next item. If the list is empty, returns NULL. To move to
//  the start of the list call nlist_first(). Advances the cursor.

void *
  nlist_next (nlist_t *self)
{
  assert (self);
  if (self->cursor)
    self->cursor = self->cursor->next;
  else
    self->cursor = self->head;
  if (self->cursor)
    return self->cursor->item;
  else
    return NULL;
}


//  --------------------------------------------------------------------------
//  Add item to the end of the list

void
  nlist_append (nlist_t *self, void *item)
{
  struct node_t *node;
  node = (struct node_t *) malloc (sizeof (struct node_t));
  node->item = item;
  if (self->tail)
    self->tail->next = node;
  else
    self->head = node;
  self->tail = node;
  node->next = NULL;
  self->size++;
  self->cursor = NULL;
}


//  --------------------------------------------------------------------------
//  Insert item at the beginning of the list

void
  nlist_push (nlist_t *self, void *item)
{
  struct node_t *node;
  node = (struct node_t *) malloc (sizeof (struct node_t));
  node->item = item;
  node->next = self->head;
  self->head = node;
  if (self->tail == NULL)
    self->tail = node;
  self->size++;
  self->cursor = NULL;
}


//  --------------------------------------------------------------------------
//  Remove item from the beginning of the list, returns NULL if none

void *
  nlist_pop (nlist_t *self)
{
  struct node_t *node = self->head;
  void *item = NULL;
  if (node) {
    item = node->item;
    self->head = node->next;
    if (self->tail == node)
      self->tail = NULL;
    free (node);
    self->size--;
  }
  self->cursor = NULL;
  return item;
}


//  --------------------------------------------------------------------------
//  Remove the item from the list, if present. Safe to call on items that
//  are not in the list.

void
  nlist_remove (nlist_t *self, void *item)
{
  struct node_t *node, *prev = NULL;

  //  First off, we need to find the list node.
  for (node = self->head; node != NULL; node = node->next) {
    if (node->item == item)
      break;
    prev = node;
  }
  if (node) {
    if (prev)
      prev->next = node->next;
    else
      self->head = node->next;

    if (node->next == NULL)
      self->tail = prev;

    free (node);
    self->size--;
    self->cursor = NULL;
  }
}


//  --------------------------------------------------------------------------
//  Make copy of itself

nlist_t *
  nlist_copy (nlist_t *self)
{
  nlist_t *copy;
  struct node_t *node;
  if (!self)
    return NULL;

  copy = nlist_new ();
  assert (copy);
  
  for (node = self->head; node; node = node->next)
    nlist_append (copy, node->item);
  return copy;
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
  nlist_size (nlist_t *self)
{
  return self->size;
}

#endif