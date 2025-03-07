#include "inc/hw_memmap.h"
#include "inc/hw_wdt.h"
#include "driverlib/watchdog.h"
#include "driverlib/prcm.h"
#include "driverlib/ioc.h"
#include "driverlib/gpio.h"
#include "driverlib/cpu.h"

/**
 * Program: main.c
 * Name: Darion Kwasnitza
 * ID: 3122890
 */
int main(void)
{
    // watchdog timer
    WatchdogResetDisable();

    // power domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();

    while(! PRCMLoadGet());

    IOCPinTypeGpioOutput(IOID_6);
    IOCPinTypeGpioOutput(IOID_7);

    while(1){
        // turn red on for 400ms (using DRA)
        HWREG(GPIO_BASE + GPIO_O_DOUT7_4) = HWREG(GPIO_BASE + GPIO_O_DOUT7_4) | 0x00010000; // red on
        CPUdelay(6400000);

        // turn off both for 600 ms
        GPIO_clearDio(6);
        GPIO_clearDio(7);
        CPUdelay(9600000);

        // turn on green for 2 seconds
        GPIO_setDio(7);
        GPIO_clearDio(6);
        CPUdelay(32000000);

        // turn off both for 1 second
        GPIO_clearDio(7);
        GPIO_clearDio(6);
        CPUdelay(16000000);

        // will repeat due to while loop
    }
    return 0;
}
