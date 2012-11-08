#include "stdint.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"


#define SERVO_MIN 1000
#define SERVO_MAX 2000

// Use ROM_* Functions wherever possible to save program space

int main(void) {
    // Enable FPU for interrupt routines
    ROM_FPULazyStackingEnable();

    // Set clock to 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    uint32_t cyclesPerSecond = ROM_SysCtlClockGet();
    uint32_t pwmPeriod = (cyclesPerSecond / 1000) * 20; // Pulse every 20ms
    uint32_t pwmLoad = SERVO_MIN;

    // Turn off LEDs
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    // Configure PB0 as T2CCP1 (PWM)
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_GPIOPinConfigure(GPIO_PB0_T2CCP0);
    ROM_GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_0);

    // Configure timer for PWM
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, pwmPeriod -1);
    ROM_TimerMatchSet(TIMER2_BASE, TIMER_A, pwmPeriod - pwmLoad); // PWM
    ROM_TimerEnable(TIMER2_BASE, TIMER_A);
    

    while(1) {
        uint8_t ledOn = 1;

        for(pwmLoad=SERVO_MIN; pwmLoad<SERVO_MAX; pwmLoad += 10) {
            ROM_SysCtlDelay((SysCtlClockGet() / 3) / 4);
            ROM_TimerMatchSet(TIMER0_BASE, TIMER_A, pwmPeriod - pwmLoad);

            ledOn = !ledOn;
            if(ledOn)
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2);
            else
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
        }
        
        for(pwmLoad=SERVO_MAX; pwmLoad>SERVO_MIN; pwmLoad -= 10) {
            ROM_SysCtlDelay((SysCtlClockGet() / 3) / 4);
            ROM_TimerMatchSet(TIMER0_BASE, TIMER_A, pwmPeriod - pwmLoad);

            ledOn = !ledOn;
            if(ledOn)
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
            else
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
        }
    }
}
