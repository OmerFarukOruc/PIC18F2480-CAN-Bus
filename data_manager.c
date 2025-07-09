/**
 * @file data_manager.c
 * @author oruc
 * @date 2025-07-09
 * @brief Data manager implementation for CAN message storage and user function calls
 */

#include "data_manager.h"
#include "can_driver.h"

// Private variables
static data_buffer_t message_buffer;
static user_function_t registered_user_function = NULL;
static uint16_t overflow_count = 0;
static uint8_t max_buffer_usage = 0;

// Private function prototypes
static void update_buffer_statistics(void);
static data_status_t send_response_to_master(const can_message_t* response);

/**
 * @brief Initialize data manager
 */
data_status_t data_manager_init(void) {
    // Initialize buffer
    message_buffer.head = 0;
    message_buffer.tail = 0;
    message_buffer.count = 0;
    message_buffer.overflow = false;
    
    // Clear all stored messages
    for (uint8_t i = 0; i < MAX_STORED_MESSAGES; i++) {
        message_buffer.messages[i].valid = false;
        message_buffer.messages[i].id = 0;
        message_buffer.messages[i].length = 0;
        
        for (uint8_t j = 0; j < MAX_DATA_BUFFER_SIZE; j++) {
            message_buffer.messages[i].data[j] = 0;
        }
    }
    
    // Reset user function pointer
    registered_user_function = NULL;
    
    // Reset statistics
    overflow_count = 0;
    max_buffer_usage = 0;
    
    return DATA_STATUS_OK;
}

/**
 * @brief Store new CAN message in buffer
 */
data_status_t data_manager_store_message(const can_message_t* message) {
    if (message == NULL || !message->valid) {
        return DATA_STATUS_ERROR;
    }
    
    // Check if buffer is full
    if (message_buffer.count >= MAX_STORED_MESSAGES) {
        message_buffer.overflow = true;
        overflow_count++;
        
        // Overwrite oldest message (circular buffer behavior)
        message_buffer.tail = (message_buffer.tail + 1) % MAX_STORED_MESSAGES;
        message_buffer.count--;
    }
    
    // Store message at head position
    message_buffer.messages[message_buffer.head] = *message;
    message_buffer.head = (message_buffer.head + 1) % MAX_STORED_MESSAGES;
    message_buffer.count++;
    
    // Update statistics
    update_buffer_statistics();
    
    return DATA_STATUS_OK;
}

/**
 * @brief Get oldest message from buffer
 */
data_status_t data_manager_get_message(can_message_t* message) {
    if (message == NULL) {
        return DATA_STATUS_ERROR;
    }
    
    if (message_buffer.count == 0) {
        return DATA_STATUS_BUFFER_EMPTY;
    }
    
    // Get message from tail position
    *message = message_buffer.messages[message_buffer.tail];
    message_buffer.tail = (message_buffer.tail + 1) % MAX_STORED_MESSAGES;
    message_buffer.count--;
    
    return DATA_STATUS_OK;
}

/**
 * @brief Get message at specific index
 */
data_status_t data_manager_get_message_at_index(uint8_t index, can_message_t* message) {
    if (message == NULL || index >= message_buffer.count) {
        return DATA_STATUS_INVALID_INDEX;
    }
    
    // Calculate actual index in circular buffer
    uint8_t actual_index = (message_buffer.tail + index) % MAX_STORED_MESSAGES;
    *message = message_buffer.messages[actual_index];
    
    return DATA_STATUS_OK;
}

/**
 * @brief Get number of stored messages
 */
uint8_t data_manager_get_message_count(void) {
    return message_buffer.count;
}

/**
 * @brief Check if buffer is full
 */
bool data_manager_is_buffer_full(void) {
    return (message_buffer.count >= MAX_STORED_MESSAGES);
}

/**
 * @brief Check if buffer is empty
 */
bool data_manager_is_buffer_empty(void) {
    return (message_buffer.count == 0);
}

/**
 * @brief Clear all stored messages
 */
void data_manager_clear_buffer(void) {
    message_buffer.head = 0;
    message_buffer.tail = 0;
    message_buffer.count = 0;
    message_buffer.overflow = false;
    
    // Mark all messages as invalid
    for (uint8_t i = 0; i < MAX_STORED_MESSAGES; i++) {
        message_buffer.messages[i].valid = false;
    }
}

/**
 * @brief Register user function to be called when new data arrives
 */
void data_manager_register_user_function(user_function_t user_func) {
    registered_user_function = user_func;
}

/**
 * @brief Call registered user function with new message
 */
void data_manager_call_user_function(const can_message_t* message) {
    if (registered_user_function != NULL && message != NULL) {
        registered_user_function(message);
    }
}

/**
 * @brief Update buffer statistics
 */
static void update_buffer_statistics(void) {
    if (message_buffer.count > max_buffer_usage) {
        max_buffer_usage = message_buffer.count;
    }
}

/**
 * @brief Get buffer statistics
 */
void data_manager_get_statistics(uint16_t* overflow_count_ptr, uint8_t* max_usage_ptr) {
    if (overflow_count_ptr != NULL) {
        *overflow_count_ptr = overflow_count;
    }
    if (max_usage_ptr != NULL) {
        *max_usage_ptr = max_buffer_usage;
    }
}

/**
 * @brief Reset buffer statistics
 */
void data_manager_reset_statistics(void) {
    overflow_count = 0;
    max_buffer_usage = message_buffer.count;
}

/**
 * @brief Process received CAN message
 */
void data_manager_process_message(const can_message_t* message) {
    if (message == NULL || !message->valid) {
        return;
    }
    
    // Check message ID to determine processing
    if (message->id == CAN_DATA_MSG_ID) {
        // Data message - store and call user function
        data_manager_store_message(message);
        data_manager_call_user_function(message);
    }
    else if (message->id == CAN_REQUEST_MSG_ID) {
        // Master request - handle and respond
        can_message_t response;
        if (data_manager_handle_master_request(message, &response) == DATA_STATUS_OK) {
            send_response_to_master(&response);
        }
    }
}

/**
 * @brief Handle master request
 */
data_status_t data_manager_handle_master_request(const can_message_t* request_message, 
                                                 can_message_t* response_message) {
    if (request_message == NULL || response_message == NULL) {
        return DATA_STATUS_ERROR;
    }
    
    // Initialize response message
    response_message->id = CAN_RESPONSE_MSG_ID;
    response_message->valid = true;
    
    // Check request type (first byte of data)
    if (request_message->length > 0) {
        switch (request_message->data[0]) {
            case 0x01: // Request for message count
                response_message->length = 2;
                response_message->data[0] = 0x01; // Response type
                response_message->data[1] = message_buffer.count;
                break;
                
            case 0x02: // Request for specific message by index
                if (request_message->length >= 2) {
                    uint8_t index = request_message->data[1];
                    can_message_t requested_msg;
                    
                    if (data_manager_get_message_at_index(index, &requested_msg) == DATA_STATUS_OK) {
                        response_message->length = 8;
                        response_message->data[0] = 0x02; // Response type
                        response_message->data[1] = index;
                        response_message->data[2] = (uint8_t)(requested_msg.id >> 8);
                        response_message->data[3] = (uint8_t)(requested_msg.id & 0xFF);
                        response_message->data[4] = requested_msg.length;
                        response_message->data[5] = requested_msg.data[0];
                        response_message->data[6] = requested_msg.data[1];
                        response_message->data[7] = requested_msg.data[2];
                    } else {
                        response_message->length = 3;
                        response_message->data[0] = 0x02; // Response type
                        response_message->data[1] = index;
                        response_message->data[2] = 0xFF; // Error code
                    }
                } else {
                    return DATA_STATUS_ERROR;
                }
                break;
                
            case 0x03: // Request for buffer statistics
                response_message->length = 5;
                response_message->data[0] = 0x03; // Response type
                response_message->data[1] = message_buffer.count;
                response_message->data[2] = (uint8_t)(overflow_count >> 8);
                response_message->data[3] = (uint8_t)(overflow_count & 0xFF);
                response_message->data[4] = max_buffer_usage;
                break;
                
            case 0x04: // Request to clear buffer
                data_manager_clear_buffer();
                response_message->length = 2;
                response_message->data[0] = 0x04; // Response type
                response_message->data[1] = 0x00; // Success
                break;
                
            default:
                // Unknown request type
                response_message->length = 2;
                response_message->data[0] = 0xFF; // Error response
                response_message->data[1] = 0x01; // Unknown command
                break;
        }
    } else {
        return DATA_STATUS_ERROR;
    }
    
    return DATA_STATUS_OK;
}

/**
 * @brief Send response to master
 */
static data_status_t send_response_to_master(const can_message_t* response) {
    if (response == NULL) {
        return DATA_STATUS_ERROR;
    }
    
    // Send response via CAN
    can_status_t status = can_send_message(response);
    
    switch (status) {
        case CAN_STATUS_OK:
            return DATA_STATUS_OK;
        case CAN_STATUS_BUSY:
            return DATA_STATUS_ERROR; // Could retry later
        case CAN_STATUS_ERROR:
        default:
            return DATA_STATUS_ERROR;
    }
}

/**
 * @brief Prepare response message for master request
 */
data_status_t data_manager_prepare_response(uint16_t request_id, can_message_t* response) {
    if (response == NULL) {
        return DATA_STATUS_ERROR;
    }
    
    // Create a dummy request message for processing
    can_message_t dummy_request;
    dummy_request.id = CAN_REQUEST_MSG_ID;
    dummy_request.length = 2;
    dummy_request.data[0] = (uint8_t)(request_id & 0xFF);
    dummy_request.data[1] = 0;
    dummy_request.valid = true;
    
    return data_manager_handle_master_request(&dummy_request, response);
}