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

typedef struct {
    uint16_t value;
    uint16_t counter;
    unsigned long port;
    unsigned char pin;
    uint8_t enabled;
} servo_t;

#ifndef SERVO_MAX_COUNT
    #define SERVO_MAX_COUNT 12
#endif

#define SERVO_TIMER TIMER2_BASE
#define SERVO_TIMER_TRIGGER TIMER_TIMA_TIMEOUT
#define SERVO_TIMER_INTERRUPT INT_TIMER2A
#define SERVO_TIMER_A TIMER_A
#define SERVO_TIMER_PERIPH SYSCTL_PERIPH_TIMER2

#define SERVO_TIMER_RESOLUTION 5    // 5 uS

#define SERVO_MIN_VALUE 1000
#define SERVO_MAX_VALUE 2000

void servoInit(void);
void servoStart(void);
servo_t *servoAttach(unsigned long port, unsigned char pin);
void servoSet(servo_t *servo, uint32_t value);
void TimerIntHandlerServos(void);


#endif