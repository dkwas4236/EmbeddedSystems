/* Name: Darion
 * Course: CMPT 464 - Assignment 1
 * Date: Feb 27
 * ID: 3122890
 * File: header.h
 * Desc: header.h is the header file for main.c
 */

#ifndef HEADER_H_
#define HEADER_H_

#include "driverlib/ioc.h"
#include "driverlib/uart.h"
#include "driverlib/prcm.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/sys_ctrl.h"
#include "driverlib/osc.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

void setup_Interrupt(void);
void led_setup(void);
void evaluateInput(void);
void UART_Interrupt_Handler(void);
void timer_setup(void);
void interrupt_fn(void);

#endif /* HEADER_H_ */
