/*
 * presentation.c
 * exercise 1
 *
 *  Created on: Jan 30, 2025
 *      Author: Darion
 */

#include "header.h"
int status = 0;

void led_setup(void)
{
    // Power on periperal domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    // Power on the GPIO peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // enable output for the green LED
    GPIO_setOutputEnableDio(IOID_7, GPIO_OUTPUT_ENABLE);
}
//
void interrupt_fn()
{
    if (status == 0){
        TimerLoadSet(GPT0_BASE, TIMER_A, 6000000);
        // Be sure the interrupt is clear to start
        TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        // Enable the interrupt
        TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        // Enable the timer
        TimerEnable(GPT0_BASE,TIMER_A);
        //TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
        GPIO_toggleDio(7);
        status = 1;
    }
    else if (status == 1){
        TimerLoadSet(GPT0_BASE, TIMER_A, 12000000);
        // Be sure the interrupt is clear to start
        TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        // Enable the interrupt
        TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        // Enable the timer
        TimerEnable(GPT0_BASE,TIMER_A);
        //TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
        GPIO_toggleDio(7);
        status = 0;
    }
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

    // The following steps follow page 1200 in the reference
    // Configure the TIMER0
    TimerConfigure(GPT0_BASE, TIMER_CFG_ONE_SHOT); // This line will configure the timer in one shot 32 bits mode
    TimerLoadSet(GPT0_BASE, TIMER_A, 6000000); // 2 seconds will be 5B8D80

    /*
    // * Reconfigure the timer to 16 bits counting mode with prescalar 256
    HWREG(GPT0_BASE + GPT_O_CFG) = 0x4;
    // Set initial timer value
    TimerLoadSet(GPT0_BASE, TIMER_A, 0x00002DC6);
    // Set prescaler
    TimerPrescaleSet(GPT0_BASE, TIMER_A, 0x000000FF);
    */
    // Assign the interrupt handler
    TimerIntRegister(GPT0_BASE, TIMER_A, interrupt_fn);
    // Be sure the interrupt is clear to start
    TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable the interrupt
    TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable the timer
    TimerEnable(GPT0_BASE,TIMER_A);

}

void runpresentation()
{
    led_setup();
    timer_setup();
}
