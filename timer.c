/*
 * timer.c
 *
 *  Created on: Nov 20, 2017
 *      Author: samaustin
 */
#include "timer.h"
#include "msp.h"
#include "circbuf.h"

extern int countsystick;
extern uint16_t sound;

void configureSYSTICK(){
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD =375;//16kHz
}
void SysTick_Handler(){
    countsystick++;
    if(ADC14->CTL0 & (ADC14_CTL0_ENC)){
        ADC14->CTL0 |= ADC14_CTL0_SC; //Sampling and conversion star
    }
}
void configureTIMERA0(){
    TIMER_A0->R =0;         // Reset count
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | // SMCLK
                    TIMER_A_CTL_MC__UP |// Up Mode
                    TIMER_A_CTL_ID__8; // 8 divide
    TIMER_A0->CCTL[1]=TIMER_A_CCTLN_OUTMOD_3; // set/rest mode
    TIMER_A0->CCTL[3]=TIMER_A_CCTLN_OUTMOD_3;
    TIMER_A0->CCR[0] = 30000; // period of pwm
    TIMER_A0->CCR[1] = 0;
    TIMER_A0->CCR[3] = 0;
}
void configureCLOCKS(){
    CS->KEY= 0x695A;
    CS->CTL0 = 0;
    CS->CTL0 = (BIT(23) | CS_CTL0_DCORSEL_3);
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;
}
