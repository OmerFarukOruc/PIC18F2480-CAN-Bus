/**
 * @file data_manager.h
 * @author oruc
 * @date 2025-07-09
 * @brief Data manager header for CAN message storage and user function calls
 */

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "config.h"

// Data Manager Status
typedef enum {
    DATA_STATUS_OK,
    DATA_STATUS_ERROR,
    DATA_STATUS_BUFFER_FULL,
    DATA_STATUS_BUFFER_EMPTY,
    DATA_STATUS_INVALID_INDEX
} data_status_t;

// Data Storage Structure
typedef struct {
    can_message_t messages[MAX_STORED_MESSAGES];
    uint8_t head;           // Next write position
    uint8_t tail;           // Next read position
    uint8_t count;          // Number of stored messages
    bool overflow;          // Buffer overflow flag
} data_buffer_t;

// User Function Pointer Type
typedef void (*user_function_t)(const can_message_t* message);

// Function Prototypes

/**
 * @brief Initialize data manager
 * @return Data status
 */
data_status_t data_manager_init(void);

/**
 * @brief Store new CAN message in buffer
 * @param message Pointer to CAN message to store
 * @return Data status
 */
data_status_t data_manager_store_message(const can_message_t* message);

/**
 * @brief Get oldest message from buffer
 * @param message Pointer to store retrieved message
 * @return Data status
 */
data_status_t data_manager_get_message(can_message_t* message);

/**
 * @brief Get message at specific index
 * @param index Index of message to retrieve
 * @param message Pointer to store retrieved message
 * @return Data status
 */
data_status_t data_manager_get_message_at_index(uint8_t index, can_message_t* message);

/**
 * @brief Get number of stored messages
 * @return Number of messages in buffer
 */
uint8_t data_manager_get_message_count(void);

/**
 * @brief Check if buffer is full
 * @return True if buffer is full, false otherwise
 */
bool data_manager_is_buffer_full(void);

/**
 * @brief Check if buffer is empty
 * @return True if buffer is empty, false otherwise
 */
bool data_manager_is_buffer_empty(void);

/**
 * @brief Clear all stored messages
 */
void data_manager_clear_buffer(void);

/**
 * @brief Register user function to be called when new data arrives
 * @param user_func Pointer to user function
 */
void data_manager_register_user_function(user_function_t user_func);

/**
 * @brief Call registered user function with new message
 * @param message Pointer to new message
 */
void data_manager_call_user_function(const can_message_t* message);

/**
 * @brief Prepare response message for master request
 * @param request_id ID of the master request
 * @param response Pointer to store response message
 * @return Data status
 */
data_status_t data_manager_prepare_response(uint16_t request_id, can_message_t* response);

/**
 * @brief Get buffer statistics
 * @param overflow_count Pointer to store overflow count
 * @param max_usage Pointer to store maximum buffer usage
 */
void data_manager_get_statistics(uint16_t* overflow_count, uint8_t* max_usage);

/**
 * @brief Reset buffer statistics
 */
void data_manager_reset_statistics(void);

/**
 * @brief Process received CAN message
 * Called by CAN interrupt handler for new messages
 * @param message Pointer to received message
 */
void data_manager_process_message(const can_message_t* message);

/**
 * @brief Handle master request
 * @param request_message Pointer to request message
 * @param response_message Pointer to store response
 * @return Data status
 */
data_status_t data_manager_handle_master_request(const can_message_t* request_message, 
                                                 can_message_t* response_message);

#endif // DATA_MANAGER_H