/* Name: Darion
 * Course: CMPT 464 - Assignment 1
 * Date: Feb 27
 * ID: 3122890
 * File: main.c
 * Desc: main.c simulates a heating and cooling unit that
 *       can be controlled by the user via UART.
 */

#include "header.h"

// buffer for commands (assuming user knows command format)
char buffer[4];
int size;

// variables for current and chosen temperatures
int tCurrent;
int tChosen;

// flags for heating and cooling protocols
int heat;
int cool;
int stop;

// Function to setup UART interrupt with correct configuration
void setup_Interrupt(){
    // Enable Power Domain
    PRCMPowerDomainOn(PRCM_DOMAIN_SERIAL);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON);

    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_UART0);
    PRCMLoadSet();
    while (!PRCMLoadGet());

    // Enable UART Pins
    IOCPinTypeUart(UART0_BASE, IOID_2, IOID_3, IOID_19, IOID_18);

    // Disable UART
    UARTDisable(UART0_BASE);

    // UART Configuration
    UARTConfigSetExpClk(UART0_BASE, 48000000, 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    UARTHwFlowControlDisable(UART0_BASE);

    // Set FIFO Thresholds
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

    // UART interrupt handler assignment
    UARTIntRegister(UART0_BASE, UART_Interrupt_Handler);

    // Enable Interrupts
    UARTIntEnable(UART0_BASE, UART_INT_RX);
    UARTEnable(UART0_BASE);
}

// Function to setup LEDs
void led_setup(void){
    // Power on peripheral domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    // Power on the GPIO peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();
    while (!PRCMLoadGet());

    // Enable output for the green LED and the red LED
    GPIO_setOutputEnableDio(IOID_7, GPIO_OUTPUT_ENABLE);
    GPIO_setOutputEnableDio(IOID_6, GPIO_OUTPUT_ENABLE);
}

// Function to evaluate inputed user commands and heat or cool accordingly
void evaluateInput(){

    // calculate tChosen from user input but only update if between values of 20 and 30
    int userValue = ((buffer[0] - '0') * 10) + (buffer[1] - '0');
    if (userValue >= 20 && userValue <=30){
        tChosen = userValue;
    }

    if (buffer[2] == ' ' && buffer[3] == ' ' && buffer[0] == 'F' && buffer[1] == 'F'){
            stop = 1;
            // sleep aware of new UART interrupts
            UARTIntEnable(UART0_BASE, UART_INT_RX);
     }

    // check if user input ends with 2 spaces and tChosen is between 20 and 30 inclusive
    else if (buffer[2] == ' ' && buffer[3] == ' ' && tChosen >= 20 && tChosen <= 30){
        // start heating protocol based on tChosen
        if (tChosen > tCurrent){
            UARTIntEnable(UART0_BASE, UART_INT_RX);
            // set heating flag to 0
            heat = 0;
            cool = 2;
            // begin timer
            timer_setup();
        }
        // start cooling protocol based on tChosen
        else if (tChosen < tCurrent){
            UARTIntEnable(UART0_BASE, UART_INT_RX);
            // set cooling flag to 0
            cool = 0;
            heat = 2;
            // begin timer
            timer_setup();
        }
    }

    // reset buffer for next command and reset line
    size = 0;
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
}

// Function to handle UART interrupts
void UART_Interrupt_Handler(){
    if (UARTIntStatus(UART0_BASE, true) != UART_INT_RX) return;
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_TX);

    while (UARTCharsAvail(UART0_BASE)){
        // get the inputed character
        int32_t ch = UARTCharGetNonBlocking(UART0_BASE) & 0x000000FF;

        // if size of buffer is less than 4, add character to buffer
        if (size < 4){
            buffer[size] = ch;
            size++;
        }

        // if buffer is full, evaluate the input
        if (size == 4){
            evaluateInput();
        }
    }
}

// Function for the timer interrupt handler for both heating and cooling
void interrupt_fn(){
    // check if stop condition is 1
    if (stop == 1){
        TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        // set heat and cool to 2 so that heating and cooling stops
        heat = 2;
        cool = 2;
        // set stop condition to 0
        stop = 0;
        // sleep aware of new UART interrupts
        UARTIntEnable(UART0_BASE, UART_INT_RX);
    }
    // check if current temperature and desired temperature match
    else if (tCurrent == tChosen){
        // sleep aware of new UART interrupts
        UARTIntEnable(UART0_BASE, UART_INT_RX);
        PRCMPeripheralSleepEnable(PRCM_PERIPH_UART0);
        // set heat and cool to 2 so that heating and cooling stops
        heat = 2;
        cool = 2;
    }
    else if (heat == 0){
            // load timer for 600ms
            TimerLoadSet(GPT0_BASE, TIMER_A, 1800000);
            // clear interrupt
            TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the interrupt
            TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the timer
            TimerEnable(GPT0_BASE,TIMER_A);
            // set red led for 600ms and send H to UART
            GPIO_setDio(6);
            UARTCharPut(UART0_BASE, 'H');
            // change heat flag to 1
            heat = 1;
    }
    else if (heat == 1){
            // load timer for 400ms
            TimerLoadSet(GPT0_BASE, TIMER_A, 1200000);
            // clear interrupt
            TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the interrupt
            TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the timer
            TimerEnable(GPT0_BASE,TIMER_A);
            // generate random increment
            int increment = rand() % 2;
            // update tCurrent
            tCurrent = tCurrent + increment;
            // create formatted string and send to UART
            char str[15];
            sprintf(str, "(%d,%d)", tCurrent, tChosen);
            int len = strlen(str);
            for (int i = 0; i < len; i++){
                UARTCharPut(UART0_BASE, str[i]);
            }
            UARTCharPut(UART0_BASE, '\n');
            UARTCharPut(UART0_BASE, '\r');
            // turn off red LED
            GPIO_clearDio(6);
            // set heat flag to 0
            heat = 0;
    }
    else if (cool == 0){
            // load timer for 500ms
            TimerLoadSet(GPT0_BASE, TIMER_A, 1500000);
            // clear interrupt
            TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the interrupt
            TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the timer
            TimerEnable(GPT0_BASE,TIMER_A);
            // set green led for 500ms and show C on screen
            GPIO_setDio(7);
            UARTCharPut(UART0_BASE, 'C');
            // set cool flag to 1
            cool = 1;
    }
    else if (cool == 1){
            // load timer for 500ms
            TimerLoadSet(GPT0_BASE, TIMER_A, 1500000);
            // clear interrupt
            TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the interrupt
            TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
            // enable the timer
            TimerEnable(GPT0_BASE,TIMER_A);
            // generate random increment
            int increment = rand() % 2;
            // update tCurrent
            tCurrent = tCurrent - increment;
            // create formatted string and send to UART
            char str[15];
            sprintf(str, "(%d,%d)", tCurrent, tChosen);
            int len = strlen(str);
            for (int i = 0; i < len; i++){
                UARTCharPut(UART0_BASE, str[i]);
            }
            UARTCharPut(UART0_BASE, '\n');
            UARTCharPut(UART0_BASE, '\r');
            // turn off green LED
            GPIO_clearDio(7);
            // set cool flag to 0
            cool = 0;
    }
}

// Function for timer setup
void timer_setup(){
    // Power on peripheral domain
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    // Power on the TIMER0 peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER0);
    // Enable TIMER0 to continue counting while the MCU sleeps
    PRCMPeripheralSleepEnable(PRCM_PERIPH_TIMER0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // Timer = CPUClock / 16
    PRCMGPTimerClockDivisionSet(PRCM_CLOCK_DIV_16);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    // Configure the TIMER0 in ONESHOT mode
    TimerConfigure(GPT0_BASE, TIMER_CFG_ONE_SHOT);
    // 1 second = 3000000
    TimerLoadSet(GPT0_BASE, TIMER_A, 0);

    // assign the interrupt handler
    TimerIntRegister(GPT0_BASE, TIMER_A, interrupt_fn);
    // clear interrupt
    TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // enable the interrupt
    TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // enable the timer
    TimerEnable(GPT0_BASE,TIMER_A);
}

int main(void){
    setup_Interrupt();
    led_setup();
    PRCMSleep();
    return 0;
}
