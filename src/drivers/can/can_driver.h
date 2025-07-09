#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include "common/config.h"
#include <stdint.h>
#include <stdbool.h>

// CAN message structure
typedef struct {
    uint32_t id;                    // Message ID
    uint8_t data[MAX_DATA_SIZE];    // Message data
    uint8_t length;                 // Data length (0-8)
    bool extended;                  // Extended ID flag
    bool remote;                    // Remote transmission request flag
} can_message_t;

// CAN driver function prototypes
void can_init(void);
bool can_transmit(can_message_t *msg);
void can_interrupt_handler(void);

#endif // CAN_DRIVER_H