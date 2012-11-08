#define SERVO_MAX_COUNT 1

#include "stdint.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "servo.h"

servo_t *servo;

void setupServos(void) {
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

    servoInit();
    servo = servoAttach(GPIO_PORTF_BASE, GPIO_PIN_1);
    servoStart();
}

int main(void) {
    // Enable FPU for interrupt routines
    ROM_FPULazyStackingEnable();

    // Set clock to 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Setup servos and start the timer for them
    setupServos();

    while(1) {
        uint16_t servoPosition;

        for(servoPosition=SERVO_MIN_VALUE; servoPosition<SERVO_MAX_VALUE; servoPosition += 50) {
            SysCtlDelay((SysCtlClockGet() / 3));
            servoSet(servo, servoPosition);
        }
        
        for(servoPosition=SERVO_MAX_VALUE; servoPosition>SERVO_MIN_VALUE; servoPosition -= 50) {
            SysCtlDelay((SysCtlClockGet() / 3));
            servoSet(servo, servoPosition);
        }
    }
}
