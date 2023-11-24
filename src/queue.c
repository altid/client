/*
 * Lock-free & Lock-based Linked queue implementation.
 * Copyright (C) 2015 George Piskas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact: geopiskas@gmail.com
 */

#include "queue.h"
#include "atomic_ops.h"

/*
 * The five following functions handle the low-order mark bit that indicates
 * whether a node is logically deleted (1) or not (0).
 *  - is_marked_ref returns whether it is marked, 
 *  - (un)set_marked changes the mark,
 *  - get_(un)marked_ref sets the mark before returning the node.
 */
inline int
is_marked_ref(long i)
{
  return (int) (i & 0x1L);
}

inline long
unset_mark(long i)
{
  i &= ~0x1L;
  return i;
}

inline long
set_mark(long i)
{
  i |= 0x1L;
  return i;
}

inline long
get_unmarked_ref(long w)
{
  return w & ~0x1L;
}

inline long
get_marked_ref(long w)
{
  return w | 0x1L;
}

// Returns a new queue and initializes the memory pool.
lqueue_t*
queue_new()
{
  lqueue_t* l = (lqueue_t*) malloc(sizeof(lqueue_t));

  // Initialize tail.
  l->tail = malloc(sizeof(node_t));
  l->tail->val = INT_MAX;
  l->tail->next = NULL;
    
  // Initialize head.
  l->head = malloc(sizeof(node_t));
  l->head->val = INT_MIN;
  l->head->next = l->tail;

  // Initialize the memory pool and the first block.
  memptr = 0;
  mem = malloc(MEM_BLOCK_CNT*sizeof(node_t));
  mem[0] = malloc(MEM_BLOCK_SIZE*sizeof(node_t));

  return l;
}

// Deletes the entire memory pool and the entire queue.
void
queue_free(lqueue_t* l)
{
  int i = 0;
  while(mem[i] != NULL) {
    free(mem[i]);
    i += 1;
  }
  free(mem);
}

/* Adds the value to the top of the queue*/
void
queue_push(lqueue_t* l, val_t val)
{
  node_t* next = NULL;
  node_t* node;

  // Fetch and increment the global memory pointer.
  uint32_t my_memptr = FAI_U32(&memptr);
  if (my_memptr >= MEM_BLOCK_CNT) {
    SWAP_U32(&memptr, 0);
    // This can and likely will leave 0 unused
    // But it assures we don't double use it
    my_memptr = FAI_U32(&memptr);
  }
  // Figure out what block to use.
  uint32_t my_memblock = my_memptr/MEM_BLOCK_SIZE;

  // If that block is a new one, initialize it.
  if(mem[my_memblock] == NULL) {
	  node_t* tmpmem = malloc(MEM_BLOCK_SIZE*sizeof(node_t));
    // Only one succeeds to initialize it. The rest free the temporary malloc.
	  if(!CAS_PTR_BOOL(&mem[my_memblock], NULL, tmpmem))
      free(tmpmem);
	}
  // Assign n a place in memory.
  node = &mem[my_memblock][my_memptr%MEM_BLOCK_SIZE];
	node->val = val;
  node->next = NULL;

  while(1) { 
    node_t *tail = l->tail;
    node_t *next = tail->next;
    if(get_unmarked_ref(next) != NULL){
      // Walk tail ahead before attempting to set
      CAS_PTR(&(l->tail), tail, get_unmarked_ref(tail->next));
    } else {
      node_t *mark = is_marked_ref(tail->next) ? get_marked_ref(node) : node;
      if(CAS_PTR_BOOL(&(l->tail->next), tail->next, mark)){
        CAS_PTR(&(l->tail), tail, node);
        return;
      }
      next = tail->next;
      if(get_marked_ref(next) != NULL){
        CAS_PTR(&(l->tail), tail, get_unmarked_ref(next));
        break;
      }
    }

    while(1) {
      next = tail->next;
      if(is_marked_ref(next))
        break;
      node->next = next;
      if (CAS_PTR_BOOL(&(l->tail->next), next, node))
        return;
    }
    // If CAS fails, someone messed up. Retry!
  };
}

val_t
queue_pop(lqueue_t* l)
{
  node_t* head;
  node_t* next;
  do {
    head = l->head;
    next = head->next;
    // Next node is already marked, advance
    if (is_marked_ref(next)){
      // If we're at the end of the tail, just return
      if(get_unmarked_ref(next) == NULL) 
        return NULL;
      // Try to set our current head pointer to "next"
      CAS_PTR(&(l->head), get_unmarked_ref(head), next);
      continue;
    }
    if(CAS_PTR_BOOL(&(l->head), head, next))
      return head->val;
    // If CAS fails, something changed. Retry!
  } while (1);
}
