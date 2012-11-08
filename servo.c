#include "servo.h"


servo_t g_servos[SERVO_MAX_COUNT];
uint8_t g_lastServo = 0;


void servoInit(void) {
    uint8_t i;
    for(i=0; i<SERVO_MAX_COUNT; i++) {
        servo_t *servo = &g_servos[i];
        servo->value = SERVO_MIN_VALUE;
        servo->counter = SERVO_MAX_VALUE; // The pin is only set high after it reaches MAX
        servo->port = 0;
        servo->pin = 0;
        servo->enabled = 0;
    }
}

servo_t *servoAttach(unsigned long port, unsigned char pin) {
    servo_t *servo = &g_servos[g_lastServo];

    servo->enabled = 1;
    servo->port = port;
    servo->pin = pin;

    g_lastServo++;

    // TODO: Assert g_lastServo < MAX

    return servo;
}

void TimerIntHandlerServos(void) {
    // SERVO_TIMER_RESOLUTION microseconds have passed, increment each counter by that
    // to determine how long to set the pin high for


    // Clear the interrupt
    ROM_TimerIntClear(SERVO_TIMER, SERVO_TIMER_TRIGGER);

    // TODO: There's probably some issue where the register is cached and the var is not updated
    // like the volatile keyword or something
    uint8_t i;
    for(i=0; i<SERVO_MAX_COUNT; i++) {
        servo_t *servo = &g_servos[i];

        if(servo->enabled) {
            servo->counter += SERVO_TIMER_RESOLUTION;

            if(servo->counter >= servo->value) {
                // End of pulse, set low
                servo->counter = 0;
                ROM_GPIOPinWrite(servo->port, servo->pin, 0);
            } else  {
                // Beginning of pulse, set high
                ROM_GPIOPinWrite(servo->port, servo->pin, servo->pin);
            }
        }
    }
}

void servoSet(servo_t *servo, uint32_t value) {
    value = value <= SERVO_MAX_VALUE ? value : SERVO_MAX_VALUE;
    value = value <= SERVO_MIN_VALUE ? value : SERVO_MIN_VALUE;
    servo->value = value;
}

void servoStart(void) {
    ROM_SysCtlPeripheralEnable(SERVO_TIMER_PERIPH);
    ROM_IntMasterEnable();
    ROM_TimerConfigure(SERVO_TIMER, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(SERVO_TIMER, SERVO_TIMER_A, (ROM_SysCtlClockGet() / 1000000) * SERVO_TIMER_RESOLUTION);
    ROM_IntEnable(SERVO_TIMER_INTERRUPT);
    ROM_TimerIntEnable(SERVO_TIMER, SERVO_TIMER_TRIGGER);
    ROM_TimerEnable(SERVO_TIMER, SERVO_TIMER_A);
}

