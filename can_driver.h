/**
 * @file can_driver.h
 * @author oruc
 * @date 2025-07-09
 * @brief CAN bus driver header for PIC18F2480 ECAN module
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include "config.h"

// CAN Driver Status
typedef enum {
    CAN_STATUS_OK,
    CAN_STATUS_ERROR,
    CAN_STATUS_BUSY,
    CAN_STATUS_TIMEOUT
} can_status_t;

// CAN Operation Modes
typedef enum {
    CAN_MODE_NORMAL,
    CAN_MODE_LOOPBACK,
    CAN_MODE_LISTEN_ONLY,
    CAN_MODE_CONFIGURATION
} can_mode_t;

// Function Prototypes

/**
 * @brief Initialize CAN bus module
 * @return CAN status
 */
can_status_t can_init(void);

/**
 * @brief Send CAN message
 * @param message Pointer to CAN message structure
 * @return CAN status
 */
can_status_t can_send_message(const can_message_t* message);

/**
 * @brief Check if CAN message is available
 * @return True if message available, false otherwise
 */
bool can_message_available(void);

/**
 * @brief Receive CAN message
 * @param message Pointer to store received message
 * @return CAN status
 */
can_status_t can_receive_message(can_message_t* message);

/**
 * @brief Set CAN operation mode
 * @param mode Desired CAN mode
 * @return CAN status
 */
can_status_t can_set_mode(can_mode_t mode);

/**
 * @brief Get current CAN operation mode
 * @return Current CAN mode
 */
can_mode_t can_get_mode(void);

/**
 * @brief Enable CAN interrupts
 */
void can_enable_interrupts(void);

/**
 * @brief Disable CAN interrupts
 */
void can_disable_interrupts(void);

/**
 * @brief CAN interrupt service routine
 * Called from main ISR when CAN interrupt occurs
 */
void can_isr(void);

/**
 * @brief Set up CAN message filters
 * @param filter_id Filter ID number (0-5)
 * @param msg_id Message ID to filter
 * @param mask Mask for filtering
 * @return CAN status
 */
can_status_t can_set_filter(uint8_t filter_id, uint16_t msg_id, uint16_t mask);

/**
 * @brief Get CAN bus error status
 * @return Error count or status
 */
uint8_t can_get_error_status(void);

/**
 * @brief Clear CAN error flags
 */
void can_clear_errors(void);

#endif // CAN_DRIVER_H