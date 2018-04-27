/*
 * bluetooth.h
 *
 *  Created on: Dec 6, 2017
 *      Author: samaustin
 */

#include<stdint.h>

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

void configureUART();
void UART_send_n(uint8_t * data, uint32_t length);
void UART_send_byte(uint8_t data);

#endif /* BLUETOOTH_H_ */
