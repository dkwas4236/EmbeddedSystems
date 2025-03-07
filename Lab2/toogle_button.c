/*
 * toogle_button.c
 *
 *  Created on: Jan 16, 2022
 *      Author: Darion
 *      ID: 3122890
 */
#include "toogle_button.h"

void setup_driver()
{
    ///// See section 6.4 in the manual
    // Enable Peripheral power domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    // Enable GIO peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);

    // Make sure that the configurations are propagated
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // Set the GIO configuration (See page 987 in the manual)
    IOCPinTypeGpioOutput(IOID_6);
    IOCPinTypeGpioOutput(IOID_7);
}

void fn()
{
    int x = IOCIntStatus(IOID_13);
    int y = IOCIntStatus(IOID_14);

    AONBatMonEnable();

    // voltage
    if (x == (1)){
        uint32_t voltage = AONBatMonBatteryVoltageGet();
        // starting at 10 and including 0 is 11
        int i = 10;
        IOCIntClear(IOID_13);
        while (i >= 0){
            if ((voltage & (1<<i))!= 0){
                GPIO_setDio(7);
                CPUdelay(4800000);
                GPIO_clearDio(6);
                GPIO_clearDio(7);
                CPUdelay(3200000);
            }
            else {
                GPIO_setDio(6);
                CPUdelay(4800000);
                GPIO_clearDio(6);
                GPIO_clearDio(7);
                CPUdelay(3200000);
            }
            i = i - 1;
        }
    }
    // temperature
    if (y == (1)){
        uint32_t temperature = AONBatMonTemperatureGetDegC();
        // starting at 15 and including 0 is 16
        int i = 15;
        IOCIntClear(IOID_14);
        while (i >= 0){
            if ((temperature & (1<<i))!= 0){
                GPIO_setDio(7);
                CPUdelay(4800000);
                GPIO_clearDio(6);
                GPIO_clearDio(7);
                CPUdelay(3200000);
            }
            else {
                GPIO_setDio(6);
                CPUdelay(4800000);
                GPIO_clearDio(6);
                GPIO_clearDio(7);
                CPUdelay(3200000);
            }
            i = i - 1;
        }
    }
}

void input_setup()
{
    IOCPinTypeGpioInput(IOID_13);
    IOCPinTypeGpioInput(IOID_14);

    IOCIOPortPullSet(IOID_13, IOC_IOPULL_UP);
    IOCIOHystSet   (IOID_13,IOC_HYST_ENABLE);

    IOCIOPortPullSet(IOID_14, IOC_IOPULL_UP);
    IOCIOHystSet   (IOID_14,IOC_HYST_ENABLE);

    IOCIntClear(IOID_13);
    IOCIntClear(IOID_14);
    IOCIntRegister(fn);

    IOCIOIntSet(IOID_13,IOC_INT_ENABLE, IOC_FALLING_EDGE);
    IOCIOIntSet(IOID_14,IOC_INT_ENABLE, IOC_FALLING_EDGE);
}

void demoToogle()
{
    setup_driver();
    input_setup();
}




