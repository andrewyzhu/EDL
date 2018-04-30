/*
 * adc.c
 *
 *  Created on: Nov 18, 2017
 *      Author: samaustin
 */
#include "msp.h"
#include "adc.h"
#include "circbuf.h"

extern uint16_t sound;
extern CircBuf_t * PrimaryBuff;
extern int thresholdcount;
int count =0;

void configureADC(){
    //Microphone GPIO
    P5->SEL0 |= BIT5;
    P5->SEL1 |= BIT5;
    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
    REF_A->CTL0 |= REF_A_CTL0_VSEL_3| // Select internal ref = 2.5V
                   REF_A_CTL0_ON;     // Internal Reference ON
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
    ADC14->CTL0 |= ADC14_CTL0_SHT0_4|      // Sampling period of 96 ADC14CLK cycles
                   ADC14_CTL0_ON|          // ADC14 on
                   ADC14_CTL0_SHP|         // ADC14 sample-and-hold pulse-mode
                   ADC14_CTL0_CONSEQ_0|    // Single-channel, single-conversion
                   ADC14_CTL0_SSEL__SMCLK; // SMCLK Clock source
    ADC14->CTL1 = (ADC14_CTL1_RES_2); // 12 bit mode
    ADC14->IER0 |= ADC14_IER0_IE0; // Enable interrupt for MEM0
    ADC14->MCTL[0] = (ADC14_MCTLN_INCH_0 | ADC14_MCTLN_VRSEL_0);
    while(!(REF_A->CTL0 & REF_A_CTL0_GENRDY)); // Wait for ref generator to settle
    ADC14->CTL0 |= ADC14_CTL0_ENC; //Enable conversions
    NVIC_EnableIRQ(ADC14_IRQn); // Enable ADC interrupt in NVIC module

}

// Pulls the audio input and stores it in our global variable
void ADC14_IRQHandler() {
    //fills a buffer at 6kHz rate
    if(ADC14_IFGR0_IFG0){
        sound =ADC14->MEM[0];
        //filling primary buffer with audio input
        if((sound>2700) && (sound <20000)){
            thresholdcount++;
        }
        if(PrimaryBuff->filling ==1){
            add_item_to_buffer(PrimaryBuff,ADC14->MEM[0]);
        }
    }

}


