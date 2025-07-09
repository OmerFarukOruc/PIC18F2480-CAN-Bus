#include "data_manager.h"
#include "drivers/can/can_driver.h"
#include "common/config.h"
#include <string.h>

static data_entry_t data_array[DATA_ARRAY_SIZE];
static uint8_t data_count = 0;
static uint8_t data_index = 0;
static uint32_t timestamp_counter = 0;

static user_callback_t user_callback = NULL;

void data_manager_init(void) {
    for (uint8_t i = 0; i < DATA_ARRAY_SIZE; i++) {
        data_array[i].valid = false;
        data_array[i].length = 0;
        data_array[i].timestamp = 0;
        memset(data_array[i].data, 0, MAX_DATA_SIZE);
    }
    
    data_count = 0;
    data_index = 0;
    timestamp_counter = 0;
    user_callback = NULL;
}

void data_manager_process_message(can_message_t *msg) {
    if (msg == NULL) return;
    
    switch (msg->id) {
        case DATA_MSG_ID:
            data_manager_store_data(msg->data, msg->length);
            
            if (user_callback != NULL) {
                user_callback(msg->data, msg->length);
            }
            break;
            
        case MASTER_REQ_ID:
            data_manager_handle_master_request(msg->id);
            break;
            
        default:
            break;
    }
}

void data_manager_set_user_callback(user_callback_t callback) {
    user_callback = callback;
}

void data_manager_handle_master_request(uint32_t request_id) {
    for (uint8_t i = 0; i < DATA_ARRAY_SIZE; i++) {
        if (data_array[i].valid) {
            data_manager_send_data_to_master(i);
        }
    }
}

void data_manager_send_data_to_master(uint8_t index) {
    if (index >= DATA_ARRAY_SIZE || !data_array[index].valid) return;
    
    can_message_t response_msg;
    response_msg.id = DATA_MSG_ID;
    response_msg.length = data_array[index].length;
    response_msg.extended = false;
    response_msg.remote = false;
    
    memcpy(response_msg.data, data_array[index].data, data_array[index].length);
    
    can_transmit(&response_msg);
}

void data_manager_store_data(uint8_t *data, uint8_t length) {
    if (data == NULL || length == 0 || length > MAX_DATA_SIZE) return;
    
    data_array[data_index].valid = true;
    data_array[data_index].length = length;
    data_array[data_index].timestamp = timestamp_counter++;
    
    memcpy(data_array[data_index].data, data, length);
    
    data_index = (data_index + 1) % DATA_ARRAY_SIZE;
    
    if (data_count < DATA_ARRAY_SIZE) {
        data_count++;
    }
}

