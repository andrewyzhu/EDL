/*
 * gpio.c
 *
 *  Created on: Nov 18, 2017
 *      Author: samaustin
 */
#include "msp.h"
#include "gpio.h"
#include "compute.h"

#define COMPARE (uint8_t)(7)
#define RECORD (uint8_t)(8)

extern Status_t speakingStatus;

void configureGPIO(){
    //configure button
    P1->SEL0 &= ~BIT1 &(~BIT4) ;
    P1->SEL1 &= ~BIT1 &(~BIT4);
    P1->DIR &= ~BIT1 &(~BIT4);
    P1->OUT |= BIT1 |BIT4 ;
    P1->REN|= BIT1 |BIT4;
    P1->IES |= BIT1 |BIT4;
    //configure red LED
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;
    //configure rgb LEDs
    P1->SEL0 &= ~BIT0;//red
    P2->SEL1 &= ~BIT0;
    P2->DIR |= BIT0;
    P2->OUT &= ~BIT0;
    P1->SEL0 &= ~BIT1;//green
    P2->SEL1 &= ~BIT1;
    P2->DIR |= BIT1;
    P2->OUT &= ~BIT1;
    P1->SEL0 &= ~BIT2;//blue
    P2->SEL1 &= ~BIT2;
    P2->DIR |= BIT2;
    P2->OUT &= ~BIT2;
    //pwm gpio configure
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR |= BIT4;
    //photo sensor
    P1->SEL0 &= ~(BIT6);
    P1->SEL1 &= ~(BIT6);
    P1->DIR &= ~(BIT6);
    P1->REN &= ~BIT6;
    P1->IES |= BIT6;

    //interrupts enabled and flags cleared
    P1->IE |= BIT1 |BIT4 | BIT6;
    P1->IFG =0;
    NVIC_EnableIRQ(PORT1_IRQn);
}
void PORT1_IRQHandler(){
    if(P1IFG & BIT1){
        set_speaking_status(RECORD);
    }
    if(P1IFG & BIT4){
        set_speaking_status(COMPARE);
    }
    if(P1IFG & BIT6){
        if(speakingStatus.comparemode ==1){
            set_speaking_status(RECORD);
        }
        else{
            set_speaking_status(COMPARE);
        }
    }
    P1->IFG=0; //clear all flags
}

