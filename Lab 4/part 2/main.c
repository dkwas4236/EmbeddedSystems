/*
 * Program: main.c
 * Lab 4 Part 2
 * Author: Darion
 * ID: 3122890
 */

#include "driverlib/ioc.h"
#include "driverlib/uart.h"
#include "driverlib/prcm.h"
#include <stdlib.h>
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/sys_ctrl.h"
#include "driverlib/osc.h"

char buffer[10];
int in = 0;
int out = 0;
int size = 0;

// UART Interrupt
void UART_Interrupt_Handler()
{
    if(UARTIntStatus(UART0_BASE,  true) !=    UART_INT_RX) return;

    UARTIntClear    (UART0_BASE,UART_INT_RX|UART_INT_TX);

    while(UARTCharsAvail(UART0_BASE))
    {
        // store in my buffer while size < 10
        if (size < 10){
            int32_t ch =   UARTCharGetNonBlocking(UART0_BASE) & 0x000000FF;
            buffer[in] = ch;
            // increment size by 1
            size = size + 1;
            // set the new in + 1 modular 10
            in = (in +1)%10;
        }
        // discard other characters entered
        else{
            int32_t ch =   UARTCharGetNonBlocking(UART0_BASE) & 0x000000FF;
        }
    }
}

void interrupt_fn()
{
    TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
    //show temperature in minicom
    while (size > 0){
        UARTCharPut(UART0_BASE, (uint8_t) buffer[out]);
        // set the new out +1 modular 10
        out = (out +1) %10;
        // deincrement size by 1
        size = size -1;
    }
    // put newline and adjust for easier reads in minicom
    UARTCharPut(UART0_BASE, (uint8_t) '\n');
    UARTCharPut(UART0_BASE, (uint8_t) '\r');
}

void timer_setup()
{
    // Power on periperal domain
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

    // Configure the TIMER0 (Periodic 32 bits mode for 5 seconds (3000000 x 5)
    TimerConfigure(GPT0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(GPT0_BASE, TIMER_A, 15000000);

    // Clear Interrupt
    TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Assign the interrupt handler
    TimerIntRegister(GPT0_BASE, TIMER_A, interrupt_fn);
    // Enable the interrupt
    TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable the timer
    TimerEnable(GPT0_BASE,TIMER_A);
}

void setup_Interrupt()
{
    // Enable Power Domain
    PRCMPowerDomainOn(PRCM_DOMAIN_SERIAL);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON);

    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_UART0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // Enable UART Pins
    IOCPinTypeUart(UART0_BASE , IOID_2, IOID_3, IOID_19, IOID_18);

    // Disable UART
    UARTDisable(UART0_BASE);

    //UART Configuration
    UARTConfigSetExpClk(UART0_BASE,48000000,9600, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
    //
    UARTHwFlowControlDisable(UART0_BASE);

    // Set FIFO Thresholds
    UARTFIFOLevelSet    (UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

    // UART interrupt handler assignment
    UARTIntRegister(UART0_BASE,     UART_Interrupt_Handler);

    // Enable Interrupts
    UARTIntEnable(UART0_BASE , UART_INT_RX);

    //Last step
    UARTEnable(UART0_BASE);
}

int main(void)
{
    setup_Interrupt();
    timer_setup();
    PRCMSleep();
    return 0;
}

