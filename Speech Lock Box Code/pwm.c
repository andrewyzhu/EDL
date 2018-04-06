/*
 * pwm.c
 *
 *  Created on: Dec 7, 2017
 *      Author: samaustin
 */
#include "pwm.h"
#include "msp.h"
#include "timer.h"

extern uint8_t lock;


void send_pwm_unlock(){
    if(lock == 1){
        TIMER_A0->CCR[1] = 2250; //set  duty cycle
        lock =0;

    }
    else{
        TIMER_A0->CCR[1] = 750; //set  duty cycle
        lock =1;
    }
}
