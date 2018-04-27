/*
 * compute.h
 *
 *  Created on: Apr 26, 2018
 *      Author: anzh1
 */

#ifndef COMPUTE_H_
#define COMPUTE_H_

#include"fft.h"
#include"circbuf.h"
#include"mel_filterbank.h"
#include "dct.h"

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

void calculate_magnitude_and_compare(complex_t total[],float magnitude[],float fc[],float bc[],float lc[],float rc[],float compareVector[],float mel_filterbank_energy[],float mfcc[]);
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




#endif /* COMPUTE_H_ */
