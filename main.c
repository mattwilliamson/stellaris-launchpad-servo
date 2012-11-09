#include "utils/uartstdio.h"
#include "stdint.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"


#define SERVO_MIN 1000
#define SERVO_MAX 2000


unsigned int cyclesPerSecond;
unsigned int pwmPeriod;

// Use ROM_* Functions wherever possible to save program space

void setServo(unsigned int pulseWidth) {
    pulseWidth = (cyclesPerSecond / 1000000) * pulseWidth;
    ROM_TimerMatchSet(TIMER2_BASE, TIMER_A, pwmPeriod - pulseWidth);
    UARTprintf("setServo: %d\n", (pwmPeriod - pulseWidth) >> 8);
}

int main(void) {
    // Enable FPU for interrupt routines
    ROM_FPULazyStackingEnable();

    // Set clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initialize the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);

    UARTprintf("Starting Helios...\n");

    // Turn off LEDs
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    // Configure PB0 as T2CCP1 (PWM)
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_GPIOPinConfigure(GPIO_PB0_T2CCP0);
    ROM_GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_0);

    // Get Speeds for PWM
    cyclesPerSecond = ROM_SysCtlClockGet();
    pwmPeriod = (cyclesPerSecond / 50); // Pulse every 20ms

    UARTprintf("cyclesPerSecond: %d\n", cyclesPerSecond);
    UARTprintf("pwmPeriod: %d\n", pwmPeriod);

    // // Configure timer for PWM
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, pwmPeriod >> 8 -1);
    setServo(SERVO_MIN);
    ROM_TimerEnable(TIMER2_BASE, TIMER_A);
    
    unsigned int pwmLoad = SERVO_MIN;
    uint8_t ledOn = 1;

    while(1) {
        for(pwmLoad=SERVO_MIN; pwmLoad<SERVO_MAX; pwmLoad += 10) {
            ROM_SysCtlDelay((cyclesPerSecond / 3) / 100); // .01s
            setServo(pwmLoad);

            ledOn = !ledOn;
            if(ledOn)
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2);
            else
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
        }
        
        for(pwmLoad=SERVO_MAX; pwmLoad>SERVO_MIN; pwmLoad -= 10) {
            ROM_SysCtlDelay((cyclesPerSecond / 3) / 100); // .01s
            setServo(pwmLoad);

            ledOn = !ledOn;
            if(ledOn)
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
            else
                ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
        }
    }
}
