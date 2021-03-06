#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
//
// Define pin to LED color mapping.
//
//*****************************************************************************
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//
// Timer0 interrupt handler. Toggles LED
//
void
IntTimer0Handler(void)
{
    uint32_t LEDstatus = 0;
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    LEDstatus ^= ~(GPIOPinRead(GPIO_PORTF_BASE, GREEN_LED));
    GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, LEDstatus);
}

//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
//
//*****************************************************************************
int main(void)
{
    static uint32_t SW2_state = 0;
    static uint32_t SW1_state = 0;

    //
    // Setup the system clock to run at 50 Mhz from PLL with crystal reference
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //
    // Enable and wait for the port to be ready for access
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    //
    // Enable the Timer0 peripheral
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //
    // Wait for the Timer0 module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    //
    // Configure Timer0 as periodic
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(TIMER0_BASE, TIMER_A, 50000000);

    TimerIntRegister(TIMER0_BASE, TIMER_A, IntTimer0Handler);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //
    // Enable the timers.
    //
    TimerEnable(TIMER0_BASE, TIMER_BOTH);

    //
    // Configure the GPIO port for the LED operation.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = GPIO_LOCK_UNLOCKED;

    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = GPIO_LOCK_UNLOCKED;

    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_M;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = GPIO_LOCK_LOCKED;

    //
    // Loop Forever
    //
    while(1)
    {
        SW2_state = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
        if(SW2_state == 0)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);
            //
            // Enable interrupts to the processor.
            //
            IntMasterEnable();
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, ~RED_LED);
            //
            // Disable interrupts to the processor.
            //
            IntMasterDisable();
        }

        SW1_state = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
        if(SW1_state == 0)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, BLUE_LED);
            //
            // Enable interrupts to the processor.
            //
            IntMasterEnable();
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, ~BLUE_LED);
            //
            // Disable interrupts to the processor.
            //
            IntMasterDisable();
        }
    }
}
