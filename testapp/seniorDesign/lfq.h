/*
 * lfq.h
 *
 *  Created on: Feb 1, 2017
 *      Author: michael
 */

/*
 * Copyright (c) 2013, Linaro Limited
 *
 * Licensed under the 3 clause BSD license.
 */

#ifndef __LFQ_H__
#define __LFQ_H__

#include <stdbool.h>
#include <sched.h>

struct lfq {
  volatile unsigned int producer;
  volatile unsigned int consumer;
  size_t queue_size;
  void **data;
};

/* Initialize a struct lfq with a size and array of memory. */
static inline void lfq_init(struct lfq *q, size_t queue_size,
			    void **data)
{
  q->queue_size = queue_size;
  q->data = data;
  q->producer = 0;
  q->consumer = 0;
}

/* Internal function to backoff on contention. */
static inline void __lfq_backoff(void)
{
  sched_yield();
}

/* Dequeue an item from the ring. Spin waiting if it is empty. */
static inline void *lfq_dequeue(struct lfq *q)
{
  void *ret;
  while (q->producer == q->consumer)
    __lfq_backoff();
  ret = q->data[q->consumer % q->queue_size];
  q->consumer++;
  return ret;
}

/* Enqueue an item onto the ring. Spin waiting if it is full. */
static inline void lfq_queue(struct lfq *q, void *item)
{
  while (q->producer - q->consumer >= q->queue_size)
    __lfq_backoff();
  q->data[q->producer % q->queue_size] = item;
  __sync_synchronize();
  q->producer++;
}

/* Test is the queue is empty. */
static inline bool lfq_empty(struct lfq *q)
{
  return q->producer == q->consumer;
}

#endif /* LFQ_H_ */
