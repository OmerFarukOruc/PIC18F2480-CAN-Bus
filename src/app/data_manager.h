#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "common/config.h"
#include "drivers/can/can_driver.h"
#include <stdint.h>
#include <stdbool.h>

// Data storage structure
typedef struct {
    uint8_t data[MAX_DATA_SIZE];    // Message data
    uint8_t length;                 // Data length
    uint32_t timestamp;             // Simple timestamp counter
    bool valid;                     // Data validity flag
} data_entry_t;

// User function callback type
typedef void (*user_callback_t)(uint8_t *data, uint8_t length);

// Data manager function prototypes
void data_manager_init(void);
void data_manager_process_message(can_message_t *msg);
void data_manager_set_user_callback(user_callback_t callback);
void data_manager_handle_master_request(uint32_t request_id);
void data_manager_send_data_to_master(uint8_t index);
void data_manager_store_data(uint8_t *data, uint8_t length);

#endif // DATA_MANAGER_H