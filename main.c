#include "stdint.h"
#include "stdbool.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"

#define LED_PIN         GPIO_PIN_2
#define PIR_PIN         GPIO_PIN_1
#define TIMER_PERIOD    20000000  // Timer period for 1 second at a 20MHz clock

volatile uint8_t motionDetected = 0;

void Timer0AIntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    GPIOPinWrite(GPIO_PORTF_BASE, LED_PIN, 0);
}

void Setup(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, PIR_PIN);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_PIN);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // Set timer as periodic
    TimerLoadSet(TIMER0_BASE, TIMER_A, TIMER_PERIOD - 1);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

int main(void) {
    Setup();

    while (1) {
        if (GPIOPinRead(GPIO_PORTF_BASE, PIR_PIN)) {
            if (!motionDetected) {
                GPIOPinWrite(GPIO_PORTF_BASE, LED_PIN, LED_PIN);

                // Enable the timer and set motionDetected after enabling
                TimerEnable(TIMER0_BASE, TIMER_A);
                motionDetected = 1;
            }
        } else {
            if (motionDetected) {
                GPIOPinWrite(GPIO_PORTF_BASE, LED_PIN, 0);
                motionDetected = 0;
            }
        }
    }
}
