#include "msp.h"
#include <stdio.h>
#include "core_cm4.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gpio.h"
#include "adc.h"
#include "timer.h"
#include "circbuf.h"
#include "fft.h"
#include "compute.h"
#include "uart.h"
#include "pwm.h"
#include "dct.h"

#define STANDBYE (uint8_t)(3)
#define FILL (uint8_t)(1)
#define PROCESS (uint8_t)(0)
#define START (uint8_t)(4)
#define RESUME (uint8_t)(4)
#define END (uint8_t)(6)
#define COMPARE (uint8_t)(7)
#define RECORD (uint8_t)(8)

//global variables
volatile uint16_t sound;
volatile CircBuf_t * PrimaryBuff;
volatile CircBuf_t * SecondaryBuff;
volatile Status_t speakingStatus;
volatile int countsystick = 0;
volatile int fftcount=0;
volatile float forwardDifference=0;
volatile float backwardDifference=0;
volatile float leftDifference=0;
volatile float rightDifference=0;
volatile uint8_t lock=1;
volatile int NOP =1;
volatile int n = 512;
#define SAMPLES (int)(512)
#define HALF (int)(256)
volatile int i;
volatile int thresholdcount =0;

/**
 * main.c
 */
void main(void){
    __disable_interrupt();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    configureGPIO();
    configureADC();
    configureCLOCKS();
    configureSYSTICK();
    configureUART();
    configureTIMERA0();

    //creating buffers in dynamic memory
    PrimaryBuff = malloc(sizeof(CircBuf_t));
    initialize_buffer(PrimaryBuff,n);
    SecondaryBuff = malloc(sizeof(CircBuf_t));
    initialize_buffer(SecondaryBuff,n);

    float fc[26];
    float bc[26];
    float lc[26];
    float rc[26];
    float compareVector[26];
    int halfn =n/2;

    //initialize speaking status
    speakingStatus.speaking =0;
    speakingStatus.finishSpeaking =1;
    speakingStatus.recordmode =1;
    speakingStatus.comparemode =0;
    speakingStatus.forwardcommand =0;
    speakingStatus.backwardcommand =0;
    speakingStatus.leftcommand =0;
    speakingStatus.rightcommand =0;

    //initialize complex data array and output frequency magnitude array
    complex_t complexData[SAMPLES];
    complex_t total[HALF];
    float magnitude[HALF];
    float mel_filterbank_energy[26];
    float mfcc[12]
    for(i=0;i<n;i++){
        if(n<halfn){
            total[i].real =0;
            total[i].imag=0;
            magnitude[i] = 0;
        }
        if(i<26){
            mel_filterbank_energy[i]=0;
            compareVector[i] =0;
            fc[i] =0;
            bc[i] =0;
            rc[i] =0;
            lc[i] =0;
        }
        complexData[i].real = 0;
        complexData[i].imag = 0;
    }

    //creating twiddles
    float tsin[HALF];
    float tcos[HALF];
    float hamming[SAMPLES];
    int start =n-100;
    create_tables(tcos,tsin,hamming);

    //initialize buffer status
    set_buffer_status(PrimaryBuff, STANDBYE);
    set_buffer_status(SecondaryBuff,STANDBYE);


    __enable_interrupt();

    while(1){

        //testing if audio input reaches threshold
        if(speakingStatus.finishSpeaking ==1){
            test_threshold();
        }

        if(speakingStatus.speaking ==1){
            __enable_interrupts();
            if((PrimaryBuff->num_items == start) && (SecondaryBuff->standbye==1)){
                set_buffer_status(SecondaryBuff,FILL);
            }
            if(buffer_full(PrimaryBuff)==1){
                set_buffer_status(PrimaryBuff, PROCESS);
                fftCalculation(complexData,tcos,tsin,magnitude,hamming);
                for(i=0;i<halfn;i++){
                    total[i].real+= complexData[i].real;
                    total[i].imag+= complexData[i].imag;
                }
                fftcount++;
                NOP ^= BIT0; //breakpoint
            }
            if(buffer_full(SecondaryBuff)==1){
                set_buffer_status(SecondaryBuff,PROCESS);
                fftCalculation(complexData,tcos,tsin,magnitude,hamming);
                for(i=0;i<halfn;i++){
                    total[i].real+= complexData[i].real;
                    total[i].imag+= complexData[i].imag;
                }
                fftcount++;
                NOP ^= BIT0; //breakpoint
            }

            //calculating magnitude in frequency bins
            if(speakingStatus.finishSpeaking==1){
                __disable_interrupt();
                calculate_magnitude_and_compare(total,magnitude,fc,bc,lc,rc,compareVector,mel_filterbank_energy,mfcc);
                //clear buffers
                clear_buffer(PrimaryBuff);
                clear_buffer(SecondaryBuff);
                //reset buffer status
                set_buffer_status(PrimaryBuff, STANDBYE);
                set_buffer_status(SecondaryBuff,STANDBYE);
                thresholdcount =0;
                P1->OUT &= ~BIT0;//end processing
                __enable_interrupt();
            }
        }
    }//end of while(1) loop
}//end of main
