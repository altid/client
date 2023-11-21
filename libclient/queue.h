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

#ifndef Lqueue_H_ 
#define Lqueue_H_

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "atomic_ops_if.h"

#ifdef DEBUG
#define IO_FLUSH                        fflush(NULL)
#endif

typedef intptr_t val_t;

// boolean CAS_PTR for ease of coding.
#define CAS_PTR_BOOL(addr, old, new) (old == CAS_PTR(addr, old, new))

typedef struct node {
    val_t val;
    struct node *next;
} node_t;

typedef struct lqueue {
    node_t *head; // INT_MIN
    node_t *tail; // INT_MAX
} lqueue_t;

// Memory pool
node_t **mem; // Memory blocks
uint32_t memptr; // Current cell
#define MEM_BLOCK_SIZE 10000 //16KB (node_t = 16b)
#define MEM_BLOCK_CNT 100 // 1.6MB of mem max

lqueue_t* queue_new();

val_t val queue_pop(lqueue_t *q);
void queue_push(lqueue_t *q, val_t val);
void queue_free(lqueue_t *q);

#endif