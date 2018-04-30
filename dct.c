/*
 * dct.c
 *
 *  Created on: Apr 29, 2018
 *      Author: samaustin
 */
#include "dct.h"

void dct(float * mel, float * dct_result, float * mfcc){
    int i;
    int j;
    for(i = 0; i < 26; i++){
        dct_result[i] = 0;
        for(j = 0; j < 26; j++){
            dct_result[i] += mel[j]*(float)(cos((double)(3.14/26*(j+0.5)*i)));
        }
    }
    int k;
    for(k = 0; k < 12; k++){
        mfcc[k] = dct_result[k+1];
    }
}

