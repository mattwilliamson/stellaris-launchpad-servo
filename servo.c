#include "servo.h"


servo_t g_servos[SERVO_MAX_COUNT]; // Servo configurations
uint8_t g_lastServo; // Used to keep track for servoAttach
uint16_t g_pulseTime; // Keep track of how far the period has gone


// Zero out all the values for all servos
void servoInit(void) {
    uint8_t i;
    for(i=0; i<SERVO_MAX_COUNT; i++) {
        servo_t *servo = &g_servos[i];
        servo->value = SERVO_MIN_PULSE;
        servo->port = 0;
        servo->pin = 0;
        servo->state = SERVO_STATE_INIT;
    }

    g_lastServo = 0;
    g_pulseTime = 0;
}

// Configure a servo and bind to a pin
servo_t *servoAttach(unsigned long port, unsigned char pin) {
    if(g_lastServo >= SERVO_MAX_COUNT - 1){
        g_lastServo = g_lastServo - 1;
    }

    servo_t *servo = &g_servos[g_lastServo];

    servo->port = port;
    servo->pin = pin;
    servo->state |= SERVO_STATE_ENABLED;

    g_lastServo++;

    return servo;
}

// Set pulse width for servo
void servoSet(servo_t *servo, uint32_t value) {
    value = value <= SERVO_MAX_PULSE ? value : SERVO_MAX_PULSE;
    value = value >= SERVO_MIN_PULSE ? value : SERVO_MIN_PULSE;
    servo->value = value;
}

// Start the timers and interrupt frequency
void servoStart(void) {
    ROM_SysCtlPeripheralEnable(SERVO_TIMER_PERIPH);
    ROM_IntMasterEnable();
    ROM_TimerConfigure(SERVO_TIMER, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(SERVO_TIMER, SERVO_TIMER_A, (ROM_SysCtlClockGet() / 1000000) * SERVO_TIMER_RESOLUTION);
    ROM_IntEnable(SERVO_TIMER_INTERRUPT);
    ROM_TimerIntEnable(SERVO_TIMER, SERVO_TIMER_TRIGGER);
    ROM_TimerEnable(SERVO_TIMER, SERVO_TIMER_A);
}

// Triggered every SERVO_TIMER_RESOLUTION microseconds
void TimerIntHandlerServos(void) {
    // Clear the interrupt
    ROM_TimerIntClear(SERVO_TIMER, SERVO_TIMER_TRIGGER);
    
    // SERVO_TIMER_RESOLUTION microseconds have passed, increment each counter by that
    // to determine how long to set the pin high for
    g_pulseTime += SERVO_TIMER_RESOLUTION;

    if(g_pulseTime > SERVO_PERIOD) {
        g_pulseTime = 0;
    }

    // Loop through al servo configs and see if they need to be set low yet
    uint8_t i;
    for(i=0; i<SERVO_MAX_COUNT; i++) {
        servo_t *servo = &g_servos[i];
        
        if(servo->state & SERVO_STATE_ENABLED) {
            if(g_pulseTime >= servo->value) {
                // End of pulse, set low
                ROM_GPIOPinWrite(servo->port, servo->pin, 0);
            } else  {
                // Beginning of pulse, set high
                ROM_GPIOPinWrite(servo->port, servo->pin, servo->pin);
            }
        }
    }
}

