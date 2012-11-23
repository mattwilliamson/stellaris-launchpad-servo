#define SERVO_MAX_COUNT 2

#include "stdint.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"
#include "drivers/buttons.h"

#include "servo.h"

servo_t *servo;
servo_t *servo2;

void setupServos(void) {
    // PA2 & PA3
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3);

    // Zero out configs
    servoInit();

    // Bind pads to servo configs
    servo = servoAttach(GPIO_PORTA_BASE, GPIO_PIN_2);
    servo2 = servoAttach(GPIO_PORTA_BASE, GPIO_PIN_3);

    // Start the servo timers
    servoStart();
}

int main(void) {
    // Enable FPU for interrupt routines
    // ROM_FPULazyStackingEnable();
    // ROM_FPUEnable();

    // Set clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Turn off LEDs
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3, 0);

    // Setup buttons
    ButtonsInit();

    // Initialize the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);

    // Setup servos and start the timer for them
    setupServos();

    uint16_t servoPosition = SERVO_MIN_PULSE;

    // Continually check which button is pressed and move the servo position that direction
    while(1) {
        switch(ButtonsPoll(0, 0) & ALL_BUTTONS) {
            case LEFT_BUTTON:
                servoPosition -= 10;

                if(servoPosition < SERVO_MIN_PULSE) {
                    servoPosition = SERVO_MIN_PULSE;

                    // Too far, blink red
                    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
                } else {
                    // Valid position, blink green
                    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2);
                }
                break;

            case RIGHT_BUTTON:
                servoPosition += 10;

                if(servoPosition > SERVO_MAX_PULSE) {
                    servoPosition = SERVO_MAX_PULSE;
                    
                    // Too far, blink red
                    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
                } else {
                    // Valid position, blink blue
                    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
                }
                break;
        }

        // Update servo positions
        servoSet(servo, servoPosition);
        servoSet(servo2, servoPosition);

        // Wait .002s
        SysCtlDelay((SysCtlClockGet() / 3) / 500);

        // Turn off all LEDs
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
}
