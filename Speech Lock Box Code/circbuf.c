/*
 * circbuf.c
 *
 *  Created on: Nov 21, 2017
 *      Author: samaustin
 */
#include "circbuf.h"
#include <stdlib.h>
#include "msp.h"
#include "core_cm4.h"
uint8_t* initialize_buffer(volatile CircBuf_t * buf, uint32_t length){
    buf->buffer =(uint16_t*)malloc(sizeof(uint16_t)*length);
    if(buf->buffer == 0){
        return 0;
    }
    buf->length = length;
    buf->num_items = 0;
    buf->head = buf->buffer;
    buf->tail = buf->buffer;
    return 0;
}

void clear_buffer(volatile CircBuf_t * buf){
    int i;
    for(i=0; i < buf->length; i++){
        buf->buffer[i] = 0;
    }
    buf->num_items = 0;
    buf->head = buf->buffer;
    buf->tail = buf->buffer;
}

void delete_buffer(volatile CircBuf_t * buf){
    free(buf->buffer);
}

int buffer_full(volatile CircBuf_t * buf){
    if(buf->num_items == buf->length){
        return 1;
    }
    return 0;
}

int buffer_empty(volatile CircBuf_t * buf){
    if(buf->num_items == 0){
        return 1;
    }
    return 0;
}

void add_item_to_buffer(volatile CircBuf_t * buf, uint16_t item){
    if(buffer_full(buf) ==1){
        return;
    }
    else if(buffer_empty(buf)==1){ //if buffer is empty
        *(buf->tail) = item; //set tail to new item
        *(buf->head)=item;   //set head to new item
        (buf->num_items)++;  //states that there is now one item
    }
    else{ //if buffer is not full
        (buf->tail)++;       //move tail up one position
       *(buf->tail) = item; //set new item to the tail
        (buf->num_items)++;  //number of items goes up one
    }
}

uint16_t remove_item_from_buffer(volatile CircBuf_t * buf){
    uint16_t item;
    (buf->num_items)--;
    if(buffer_empty(buf)==1){
        item= *buf->head;
        (buf->head)++;
        clear_buffer(buf);
        return 0;
    }
    item= *buf->head;
    (buf->head)++;
    return item;
}
void set_buffer_status(volatile CircBuf_t * buf, uint8_t state){
    if(state == 1){
        buf->filling =1;
        buf->processing =0;
        buf->threshold =0;
        buf->standbye=0;
    }
    else if(state == 0){
        buf->filling =0;
        buf->processing=1;
        buf->threshold =0;
        buf->standbye=0;
    }
    else if(state == 2){
        buf->filling =0;
        buf->processing=0;
        buf->threshold =1;
        buf->standbye=0;
    }
    else{
        buf->filling =0;
        buf->processing=0;
        buf->threshold =0;
        buf->standbye=1;
    }
}

