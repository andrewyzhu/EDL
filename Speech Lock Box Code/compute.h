/*
 * compute.h
 *
 *  Created on: Dec 2, 2017
 *      Author: samaustin
 */
#include"fft.h"
#include"circbuf.h"

#ifndef COMPUTE_H_
#define COMPUTE_H_

typedef struct{
    volatile int speaking;
    volatile int finishSpeaking;
    volatile int recordmode;
    volatile int comparemode;
}Status_t;

void calculate_magnitude_and_compare(complex_t total[],float magnitude[],float recordVector[],float compareVector[]);
void test_threshold();
void set_speaking_status(uint8_t status);
void newline();
void ftoa(float n, char *res, int afterpoint);
void format();

#endif /* COMPUTE_H_ */
