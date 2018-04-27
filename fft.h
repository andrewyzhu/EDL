/*
 * fft.h
 *
 *  Created on: Nov 22, 2017
 *      Author: samaustin
 */

#ifndef FFT_H_
#define FFT_H_

#include "circbuf.h"

typedef struct{
    float real;
    float imag;
}complex_t;
void fftCalculation(complex_t complexData[],float tcos[],float tsin[],float magnitude[],float hamming[]);
unsigned int reverse_bits(int index, int bits);
void create_tables(float tcos[], float tsin[],float hamming[]);

#endif /* FFT_H_ */
