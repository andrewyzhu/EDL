/*
 * bluetooth.c
 *
 *  Created on: Dec 6, 2017
 *      Author: samaustin
 */
#include <stdint.h>
#include "uart.h"
#include "msp.h"

void configureUART(){
    P1->SEL0 |= BIT2 | BIT3;
    P1->SEL1 &= ~(BIT2 | BIT3);
    //configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
                      EUSCI_B_CTLW0_SSEL__SMCLK;
    EUSCI_A0->CTLW0 |= BIT7;                //select frame rate parameters and clock source
    EUSCI_A0->BRW = 78;                     //set baud rate
    EUSCI_A0->MCTLW |= BIT5 | BIT0;         //set first stage modulator bit (if needed)
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Initialize eUSCI

    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    //EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);           //Enable eUSCIA0 interrupt in NVIC
}
void UART_send_n(uint8_t * data, uint32_t length){
    int i;
    for (i=0;i<length; i++){
        UART_send_byte(data[i]);
    }
}
void UART_send_byte(uint8_t data){
     while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
     EUSCI_A0 -> TXBUF = data;
}
extern void EUSCIA0_IRQHandler(){
     if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
         EUSCI_A0->IFG &=~ EUSCI_A_IFG_RXIFG;
         while(!EUSCI_A_STATW_BUSY);// Check if the TX buffer is empty first
         //code for UART handler
     }
}
