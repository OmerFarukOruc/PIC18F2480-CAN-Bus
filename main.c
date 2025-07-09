/**
 * @file main.c
 * @author oruc
 * @date 2025-07-09
 * @brief Main function - CAN bus communication system
 */

#include "config.h"
#include "can_driver.h"
#include "data_manager.h"
#include "timer_handler.h"

// Application state
static app_state_t app_state = APP_STATE_INIT;

// Function prototypes
static void system_init(void);
static void application_loop(void);
static void user_data_callback(const can_message_t* message);
static void periodic_task_callback(void);
static void handle_error_state(void);

/**
 * @brief Main interrupt service routine
 */
void __interrupt() isr(void) {
    // Handle CAN interrupts
    if (PIR3bits.RXB0IF || PIR3bits.RXB1IF || PIR3bits.TXB0IF || 
        PIR3bits.TXB1IF || PIR3bits.TXB2IF || PIR3bits.ERRIF) {
        can_isr();
    }
    
    // Handle Timer1 interrupt
    if (PIR1bits.TMR1IF) {
        timer_isr();
    }
}

/**
 * @brief Main function
 */
int main(void) {
    // Initialize system
    system_init();
    
    // Main application loop
    application_loop();
    
    return 0;
}

/**
 * @brief Initialize system components
 */
static void system_init(void) {
    // Initialize data manager
    if (data_manager_init() != DATA_STATUS_OK) {
        app_state = APP_STATE_ERROR;
        return;
    }
    
    // Register user callback function
    data_manager_register_user_function(user_data_callback);
    
    // Initialize timer
    if (timer_init() != TIMER_STATUS_OK) {
        app_state = APP_STATE_ERROR;
        return;
    }
    
    // Register periodic task callback
    timer_register_callback(periodic_task_callback);
    
    // Initialize CAN bus
    if (can_init() != CAN_STATUS_OK) {
        app_state = APP_STATE_ERROR;
        return;
    }
    
    // Start timer
    if (timer_start() != TIMER_STATUS_OK) {
        app_state = APP_STATE_ERROR;
        return;
    }
    
    // System initialization complete
    app_state = APP_STATE_RUNNING;
}

/**
 * @brief Main application loop
 */
static void application_loop(void) {
    while (1) {
        switch (app_state) {
            case APP_STATE_INIT:
                // Should not reach here after initialization
                system_init();
                break;
                
            case APP_STATE_RUNNING:
                // Normal operation
                // Check for any maintenance tasks
                
                // Check CAN bus errors
                if (can_get_error_status() > 10) {
                    // Too many errors, try to recover
                    can_clear_errors();
                    can_init(); // Reinitialize CAN
                }
                
                // Check buffer overflow
                if (data_manager_is_buffer_full()) {
                    // Buffer management could be implemented here
                    // For now, just continue (circular buffer will overwrite)
                }
                
                // Sleep or low power mode could be implemented here
                __delay_ms(10);
                break;
                
            case APP_STATE_ERROR:
                handle_error_state();
                break;
                
            default:
                app_state = APP_STATE_ERROR;
                break;
        }
    }
}

/**
 * @brief User data callback function
 * This function is called when new CAN data is received
 */
static void user_data_callback(const can_message_t* message) {
    // This is the user-defined function mentioned in the requirements
    // It gets called automatically when new CAN data arrives
    
    if (message == NULL || !message->valid) {
        return;
    }
    
    // Example processing of received data
    // You can customize this function based on your needs
    
    // Simple example: toggle an LED based on received data
    if (message->length > 0) {
        if (message->data[0] & 0x01) {
            // Turn on LED or set output pin
            // Example: LATAbits.LATA0 = 1;
        } else {
            // Turn off LED or clear output pin
            // Example: LATAbits.LATA0 = 0;
        }
    }
    
    // Another example: process specific data patterns
    if (message->length >= 4) {
        uint32_t data_value = ((uint32_t)message->data[0] << 24) |
                              ((uint32_t)message->data[1] << 16) |
                              ((uint32_t)message->data[2] << 8) |
                              ((uint32_t)message->data[3]);
        
        // Process the 32-bit data value
        // Add your custom processing logic here
    }
}

/**
 * @brief Periodic task callback function
 * This function is called at regular intervals by the timer
 */
static void periodic_task_callback(void) {
    // This function demonstrates optional periodic data transmission
    // as mentioned in the requirements
    
    static uint8_t periodic_counter = 0;
    static uint32_t last_status_time = 0;
    
    // Example: Send periodic status message every 5 seconds
    if (timer_elapsed(last_status_time, 5000)) {
        can_message_t status_msg;
        
        status_msg.id = 0x300;  // Status message ID
        status_msg.length = 8;
        status_msg.data[0] = 0x01;  // Status message type
        status_msg.data[1] = periodic_counter++;
        status_msg.data[2] = data_manager_get_message_count();
        status_msg.data[3] = can_get_error_status();
        status_msg.data[4] = (uint8_t)(timer_get_tick_count() >> 24);
        status_msg.data[5] = (uint8_t)(timer_get_tick_count() >> 16);
        status_msg.data[6] = (uint8_t)(timer_get_tick_count() >> 8);
        status_msg.data[7] = (uint8_t)(timer_get_tick_count());
        status_msg.valid = true;
        
        // Send status message
        can_send_message(&status_msg);
        
        last_status_time = timer_get_tick_count();
    }
    
    // Example: Watchdog reset or other periodic maintenance
    // Add your periodic tasks here
}

/**
 * @brief Handle error state
 */
static void handle_error_state(void) {
    // Error handling logic
    // This could include:
    // - Resetting modules
    // - Sending error messages
    // - Entering safe mode
    // - Logging errors
    
    // Simple error recovery: try to reinitialize
    static uint8_t error_count = 0;
    
    error_count++;
    
    if (error_count < 3) {
        // Try to recover
        __delay_ms(1000);  // Wait 1 second
        system_init();     // Reinitialize
    } else {
        // Too many errors, enter safe mode
        can_disable_interrupts();
        timer_disable_interrupts();
        
        // Flash error LED or send error message
        while (1) {
            // Error indication loop
            // Could flash an LED here
            __delay_ms(500);
        }
    }
}
