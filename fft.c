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
#define BITS (int)(10)

extern int i;
extern int n;
extern int NOP;
extern int srate;
extern int fftcount;
extern Status_t speakingStatus;
extern CircBuf_t * PrimaryBuff;

void create_tables(float tcos[], float tsin[],float hamming[]){
    for(i=0;i<n;i++){
        if(i<n/2){
            tcos[i] = cos(TWO_PI * i / n);
            tsin[i] = sin(TWO_PI * i /n);
        }
        hamming[i] = 0.54-0.46*cos(TWO_PI*i/n);
    }
}
//the reverse_bits function was provided from http://www.geeksforgeeks.org/write-an-efficient-c-program-to-reverse-bits-of-a-number/
int reverse_bits(int num, int bits){
    int reverse_num = 0;
    int h;
    for (h = 0; h < bits; h++)
    {
        if((num & (1 << h)))
           reverse_num |= 1 << ((bits - 1) - h);
   }
    return reverse_num;
}


void fftCalculation(complex_t total[],float tcos[],float tsin[],float hamming[]){
    float holdreal,holdimag,temp; //holding value
    int  k, j,l,size,halfspan,stepspan,b;
    complex_t complexData[512];
    //transfer data in buffer to complex_data array
    for(i=0;i<n;i++){
        complexData[i].real =0;
        complexData[i].imag =0;
    }
    for(i=0;i<n;i++){
        complexData[i].real = remove_item_from_buffer(PrimaryBuff);
        complexData[i].real *= hamming[i];
    }

    for (b = 0; b < n; b++) {
        int new = reverse_bits(b, 9);
        if (new > b){
            temp = complexData[b].real;
            complexData[b].real = complexData[new].real;
            complexData[new].real= temp;
            temp = complexData[b].imag;
            complexData[b].imag = complexData[new].imag;
            complexData[new].imag = temp;
        }
     }
    //fft conversion
    for(size = 2; size <= n; size *= 2){
        halfspan = size/2;
        stepspan = n/size;
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
        if (size == n)  // Prevent overflow in 'size *= 2'
                    break;
    }
    for(i=0;i<(n/2);i++){
        total[i].real += complexData[i].real;
        total[i].imag += complexData[i].imag;
    }

}
