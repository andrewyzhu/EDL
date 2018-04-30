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
#include "dct.h"

#define STANDBYE (uint8_t)(3)
#define FILL (uint8_t)(1)
#define PROCESS (uint8_t)(0)
#define START (uint8_t)(4)
#define RESUME (uint8_t)(4)
#define END (uint8_t)(6)
#define COMPARE (uint8_t)(7)
#define RECORD (uint8_t)(8)
#define BITS (int)(10)

//global variables
volatile uint16_t sound;
volatile CircBuf_t * PrimaryBuff;
volatile Status_t speakingStatus;
volatile int countsystick = 0;
volatile float forwardDifference=0;
volatile float backwardDifference=0;
volatile float leftDifference=0;
volatile float rightDifference=0;
volatile int NOP =1;
volatile int n = 512;
volatile int binSize =26;
volatile int buffsize =7168;
#define SAMPLES (int)(512)
#define HALF (int)(256)
#define DCTSIZE (uint8_t)(26)
volatile int i;
volatile int thresholdcount =0;

/**
 * main.c
 */
void main(void){
    __disable_interrupt();
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	configureGPIO();
	configureADC();
	configureCLOCKS();
	configureSYSTICK();
	configureUART();
	configureTIMERA0();

    //creating buffers in dynamic memory
    PrimaryBuff = malloc(sizeof(CircBuf_t));
    initialize_buffer(PrimaryBuff,buffsize);

    float fc[DCTSIZE];
    float bc[DCTSIZE];
    float lc[DCTSIZE];
    float rc[DCTSIZE];
    float compareVector[DCTSIZE];
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
	complex_t total[HALF];
	float mfcc[DCTSIZE];
	for(i=0;i<n;i++){
	    if(i<halfn){
	        total[i].real=0;
	        total[i].imag=0;
	    }
	    if(i<binSize){
	        mfcc[i]=0;
            compareVector[i] =0;
            fc[i] =0;
            bc[i] =0;
            rc[i] =0;
            lc[i] =0;
	    }
	}

	//creating twiddles
	float tsin[HALF];
	float tcos[HALF];
	float hamming[SAMPLES];
	int w,m;
	create_tables(tcos,tsin,hamming);

	//initialize buffer status
	set_buffer_status(PrimaryBuff, STANDBYE);

	__enable_interrupt();

	while(1){

	    //testing if audio input reaches threshold
	    if(speakingStatus.finishSpeaking ==1){
	        test_threshold();
	    }

	    if(speakingStatus.speaking ==1){
	        if(buffer_full(PrimaryBuff)==1){
	            __disable_interrupt();
	            set_buffer_status(PrimaryBuff, PROCESS);
                while(speakingStatus.speaking ==1){
                    fftCalculation(total,tcos,tsin,hamming);
                    NOP^=BIT0;
                }
                calculate_magnitude_and_compare(total,fc,bc,lc,rc,compareVector,mfcc);
                P1->OUT &= ~BIT0;//end processing
                thresholdcount =0;
                clear_buffer(PrimaryBuff);
                set_buffer_status(PrimaryBuff, STANDBYE);
                set_speaking_status(END);
                for(m=0;m<halfn;m++){
                    total[m].real= 0;
                    total[m].imag= 0;
                }
	            __enable_interrupt();
	        }
	    }
	}//end of while(1) loop
}//end of main
