#ifndef TIMER_HANDLER_H
#define TIMER_HANDLER_H

#include "common/config.h"
#include <stdint.h>
#include <stdbool.h>

// Timer callback function type
typedef void (*timer_callback_t)(void);

// Timer handler function prototypes
void timer_init(void);
void timer_start(void);
void timer_interrupt_handler(void);

#endif // TIMER_HANDLER_H