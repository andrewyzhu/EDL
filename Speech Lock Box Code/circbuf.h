/*
 * circbuf.h
 *
 *  Created on: Nov 21, 2017
 *      Author: samaustin
 */

#ifndef CIRCBUF_H_
#define CIRCBUF_H_

#include "circbuf.h"
#include <stdint.h>
#include "msp.h"

typedef struct{
    volatile uint16_t* buffer; //Pointer to the base of the buffer in heap
    volatile uint16_t* head; //Pointer to first item
    volatile uint16_t* tail; //Pointer to last item
    volatile uint32_t num_items; //Number of items in buffer
    volatile uint32_t length;//Max number of items the buffer can hold
    volatile uint8_t filling;
    volatile uint8_t processing;
    volatile uint8_t threshold;
    volatile uint8_t standbye;
} CircBuf_t;

uint8_t* initialize_buffer(volatile CircBuf_t * buf, uint32_t length);
void clear_buffer(volatile CircBuf_t * buf);
void delete_buffer(volatile CircBuf_t * buf);
int buffer_full(volatile CircBuf_t * buf);
int buffer_empty(volatile CircBuf_t * buf);
void add_item_to_buffer(volatile CircBuf_t * buf, uint16_t item);
uint16_t remove_item_from_buffer(volatile CircBuf_t * buf);
void set_buffer_status(volatile CircBuf_t * buf, uint8_t state);

#endif /* CIRCBUF_H_ */
