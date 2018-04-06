/*
 * fft.c
 *
 *  Created on: Nov 22, 2017
 *      Author: samaustin
 */
#include "fft.h"
#include "msp.h"
#include "circbuf.h"
#include "compute.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define TWO_PI 6.283185
#define STANDBYE (uint8_t)(3)
#define THRESHOLD (uint8_t)(2)
#define FILL (uint8_t)(1)
#define PROCESS (uint8_t)(0)
#define START (uint8_t)(4)
#define RESUME (uint8_t)(4)
#define END (uint8_t)(6)
#define COMPARE (uint8_t)(7)
#define RECORD (uint8_t)(8)
#define IDEL (uint8_t)(9)
#define BITS (int)(9)

extern int i;
extern int n;
extern int NOP;
extern int fftcount;
extern Status_t speakingStatus;
extern CircBuf_t * PrimaryBuff;
extern CircBuf_t * SecondaryBuff;

void create_tables(float tcos[], float tsin[],float hamming[]){
    for(i=0;i<n/2;i++){
        if(i<n/2){
            tcos[i] = cos(TWO_PI * i / n);
            tsin[i] = sin(TWO_PI * i /n);
        }
        hamming[i] = 0.54-0.46*cos(TWO_PI*i/n);
    }
}

unsigned int reverse_bits(int num, int bits){
    unsigned int reverse_num = 0;
    int h;
    for (h = 0; h < bits; h++)
    {
        if((num & (1 << h)))
           reverse_num |= 1 << ((bits - 1) - h);
   }
    return reverse_num;
}

void fftCalculation( complex_t complexData[],float tcos[],float tsin[],float magnitude[],float hamming[]){
    float holdreal,hold,holdimag; //holding value
    int  k, j,l,size,big;


    //transfer data in buffer to complex_data array
    big =0;
    for(i=0;i<n;i++){
        if(PrimaryBuff->processing ==1){
            complexData[i].real =remove_item_from_buffer(PrimaryBuff);
        }
        if(SecondaryBuff->processing ==1){
            complexData[i].real =remove_item_from_buffer(SecondaryBuff);
        }
        if(big<complexData[i].real){
            big = complexData[i].real;
        }
        if(i>1){
            complexData[i].real -= (0.95*complexData[i-1].real);
            //complexData[i].real *=hamming[i];
        }
        complexData[i].imag =0;

    }
    if(big<10000){
        __disable_interrupt();
        set_speaking_status(END);
    }
    else{
        set_speaking_status(RESUME);
    }

    //reversing bits for fft conversion
    for(i=0;i<n;i++){
        int new_reversed = reverse_bits(i,BITS);
        if(new_reversed >i){
            hold = complexData[i].real;
            complexData[i].real = complexData[new_reversed].real;
            complexData[new_reversed].real = hold;
            hold = complexData[i].imag;
            complexData[i].imag =complexData[new_reversed].imag;
            complexData[new_reversed].imag = hold;
        }
    }

    //fft conversion
    for(size = 2; size <= n; size *= 2){
        int halfspan = size/2;
        int stepspan = n/size;
        for(i =0;i<n; i+=size){
            for(j = i, k = 0 ; j < (i + halfspan) ; j++, k += stepspan){
                l =j + halfspan;
                holdreal = complexData[l].real * tcos[k]
                         + complexData[l].imag * tsin[k];
                holdimag = complexData[l].imag * tcos[k]
                         - complexData[l].real * tsin[k];
                complexData[l].real = complexData[j].real - holdreal;
                complexData[l].imag =complexData[j].imag - holdimag;
                complexData[j].real = complexData[j].real + holdreal;
                complexData[j].imag =complexData[j].imag + holdimag;
            }
        }
    }
}
