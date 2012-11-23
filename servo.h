#ifndef __SERVO_H__
#define __SERVO_H__

#include "stdint.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

typedef enum {
    SERVO_STATE_INIT = 0,
    SERVO_STATE_ENABLED = 1
} SERVO_STATE;

typedef struct {
    uint16_t value;
    unsigned long port;
    unsigned char pin;
    uint8_t state;
} servo_t;

#ifndef SERVO_MAX_COUNT
    #define SERVO_MAX_COUNT 12
#endif

#define SERVO_TIMER TIMER2_BASE
#define SERVO_TIMER_TRIGGER TIMER_TIMA_TIMEOUT
#define SERVO_TIMER_INTERRUPT INT_TIMER2A
#define SERVO_TIMER_A TIMER_A
#define SERVO_TIMER_PERIPH SYSCTL_PERIPH_TIMER2

#define SERVO_TIMER_RESOLUTION 10 // Microseconds

#define SERVO_MIN_PULSE 1000
#define SERVO_MAX_PULSE 2000
#define SERVO_PERIOD 20000

void servoInit(void);
void servoStart(void);
servo_t *servoAttach(unsigned long port, unsigned char pin);
void servoSet(servo_t *servo, uint32_t value);
void TimerIntHandlerServos(void);


#endif
