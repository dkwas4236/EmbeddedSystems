

/**
 * Program: main.c
 * Lab 4 Part 1
 * Author: Darion
 * ID: 3122890
 */
#include "driverlib/ioc.h"
#include "driverlib/uart.h"
#include "driverlib/prcm.h"
#include "driverlib/aon_batmon.h"
#include "string.h"
#include "stdio.h"
#include "driverlib/interrupt.h"
#include <stdlib.h>
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/sys_ctrl.h"
#include "driverlib/osc.h"

// global variable to hold temperature
char str[5];

void setup_No_Interrupt()
{
    // Enable Power Domain
    PRCMPowerDomainOn(PRCM_DOMAIN_SERIAL);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON);

    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // Enable UART Pins
    IOCPinTypeUart(UART0_BASE , IOID_2, IOID_3, IOID_19, IOID_18);

    // Disable UART
    UARTDisable(UART0_BASE);

    //UART Configuration (9600 baud rate, 1 stop bit, no parity bit, 8 data bits)
    UARTConfigSetExpClk(UART0_BASE,48000000,9600, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);

    UARTHwFlowControlDisable(UART0_BASE);

    //Last step
    UARTEnable(UART0_BASE);
}

void interrupt_fn(){
    // enable temperature reading
    AONBatMonEnable();

    // variables for first, and second character
    int first;
    int second;

    // get the temperature
    uint32_t temperature = AONBatMonTemperatureGetDegC();

    // get the first digit
    first = (temperature/10);

    // get the second digit
    second = (temperature%10);

    // structure string in format XX C\n
    str[0] = first +'0';
    str[1] = second + '0';
    str[2] = ' ';
    str[3] = 'C';
    str[4] = '\n';
    str[5] = '\r';

    // show temperature in minicom
    for (int i = 0; i < 6; i++){
        UARTCharPut(UART0_BASE, (uint8_t) str[i]);
    }
    // make a timer for 5 seconds (12000 x 5)
    TimerLoadSet(GPT0_BASE, TIMER_A, 60000);
    // Clear Interrupt
    TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable Interrupt
    TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable the timer
    TimerEnable(GPT0_BASE,TIMER_A);
}

void timer_setup()
{
    // Power on peripheral domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    // Power on the TIMER0 peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER0);

    // Enable TIMER0 to continue counting while the MCU sleeps
    PRCMPeripheralSleepEnable(PRCM_PERIPH_TIMER0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // This line will result that the input clock to the Timer = CPUClock / 16
    PRCMGPTimerClockDivisionSet(PRCM_CLOCK_DIV_16);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // Configure the TIMER0 (one shot 32 bits mode)
    TimerConfigure(GPT0_BASE, TIMER_CFG_ONE_SHOT);

    // Reconfigure the timer to 16 bits counting mode with prescalar 256
    HWREG(GPT0_BASE + GPT_O_CFG) = 0x4;
    // Set initial timer value (12000 for 250)
    TimerLoadSet(GPT0_BASE, TIMER_A, 12000);
    // Set prescaler (250 - 1)
    TimerPrescaleSet(GPT0_BASE, TIMER_A, 249);

    // Assign the interrupt handler to interrupt_fn function
    TimerIntRegister(GPT0_BASE, TIMER_A, interrupt_fn);
    // Clear Interrupt
    TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable Interrupt
    TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable the timer
    TimerEnable(GPT0_BASE,TIMER_A);
}

int main(void)
{
    setup_No_Interrupt();
    timer_setup();
    PRCMSleep();
	return 0;
}
