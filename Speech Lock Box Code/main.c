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
volatile int soundcount=0;
volatile int fftcount=0;
volatile float averageDifference=0;
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
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
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

    float recordVector[SAMPLES];
    float compareVector[SAMPLES];

    //initialize speaking status
    speakingStatus.speaking =0;
    speakingStatus.finishSpeaking =1;
    speakingStatus.recordmode =1;
    speakingStatus.comparemode =0;

	//initialize complex data array and output frequency magnitude array
	complex_t complexData[SAMPLES];
	complex_t total[SAMPLES];
	float magnitude[SAMPLES];
	for(i=0;i<n;i++){
	    total[i].real =0;
	    total[i].imag=0;
	    complexData[i].real = 0;
        complexData[i].imag = 0;
        magnitude[i] = 0;
        compareVector[i] =0;
        recordVector[i] =0;
	}

	//creating twiddles
	float tsin[HALF];
	float tcos[HALF];
	float hamming[SAMPLES];
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

	    //enters when threshold is meet and exists after 150 fft calculations
	    //150 fft calculations takes roughly 3 seconds to complete
	    //The 3 seconds is the time given to say the password
	    //dual buffer used so while one buffer is capturing new data the other is performing fft conversion
	    //this allows us to never miss any new audio data while in the fft conversion
	    if(speakingStatus.speaking ==1){
	        //start processing PrimaryBuff and start filling SecondaryBuff
	        if((buffer_full(PrimaryBuff)==1) && (buffer_empty(SecondaryBuff)==1)){
	            set_buffer_status(SecondaryBuff,FILL);
	            set_buffer_status(PrimaryBuff, PROCESS);
	            fftCalculation(complexData,tcos,tsin,magnitude,hamming);
                for(i=0;i<n;i++){
                    total[i].real+= complexData[i].real;
                    total[i].imag+= complexData[i].imag;
                }
                fftcount++;
	            NOP ^= BIT0; //breakpoint
	        }

	        //starts processing SecondaryBuff and filling PrimaryBuff
	        if((buffer_full(SecondaryBuff)==1) && (buffer_empty(PrimaryBuff)==1)){
	            set_buffer_status(PrimaryBuff, FILL);
	            set_buffer_status(SecondaryBuff,PROCESS);
	            fftCalculation(complexData,tcos,tsin,magnitude,hamming);
                for(i=0;i<n;i++){
                    total[i].real+= complexData[i].real;
                    total[i].imag+= complexData[i].imag;
                }
                fftcount++;
                NOP ^= BIT0; //breakpoint
	        }

	        //calculating magnitude in frequency bins
	        if(speakingStatus.finishSpeaking==1){
	            __disable_interrupt();
	            calculate_magnitude_and_compare(total,magnitude,recordVector,compareVector);
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
