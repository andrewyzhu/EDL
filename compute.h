/*
 * compute.h
 *
 *  Created on: Dec 2, 2017
 *      Author: samaustin
 */
#include"fft.h"
#include"circbuf.h"
#include"mel_filterbank.h"

#ifndef COMPUTE_H_
#define COMPUTE_H_

typedef struct{
    volatile int speaking;
    volatile int finishSpeaking;
    volatile int recordmode;
    volatile int comparemode;
    volatile int forwardcommand;
    volatile int backwardcommand;
    volatile int leftcommand;
    volatile int rightcommand;
}Status_t;

void calculate_magnitude_and_compare(complex_t total[],float fc[],float bc[],float lc[],float rc[],float compareVector[],float mfcc[]);
void test_threshold();
void set_speaking_status(uint8_t status);
void newline();
void ftoa(float n, char *res, int afterpoint);
void format();
void forwardformat();
void backwardformat();
void leftformat();
void rightformat();
void print_diff(float diff1,float diff2,float diff3,float diff4);
void look_for_match(float forwardDifference,float backwardDifference,float leftDifference, float rightDifference);

#endif /* COMPUTE_H_ */
