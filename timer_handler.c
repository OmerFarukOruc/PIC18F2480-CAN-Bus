/**
 * @file timer_handler.c
 * @author oruc
 * @date 2025-07-09
 * @brief Timer handler implementation for periodic operations
 */

#include "timer_handler.h"

// Private variables
static volatile uint32_t tick_count = 0;
static volatile bool timer_running = false;
static uint16_t timer_period_ms = TIMER_PERIOD_MS;
static timer_callback_t registered_callback = NULL;

// Timer1 configuration for 1ms tick with 20MHz crystal
#define TIMER1_RELOAD_VALUE 0xB1E0  // For 1ms @ 20MHz with 1:8 prescaler

/**
 * @brief Initialize timer module
 */
timer_status_t timer_init(void) {
    // Configure Timer1 for 1ms interrupts
    // Timer1 configuration: 16-bit mode, 1:8 prescaler, internal clock
    T1CONbits.TMR1ON = 0;    // Turn off Timer1
    T1CONbits.TMR1CS = 0;    // Use internal clock (Fosc/4)
    T1CONbits.T1SYNC = 0;    // Synchronize external clock input
    T1CONbits.T1OSCEN = 0;   // Timer1 oscillator disabled
    T1CONbits.T1CKPS = 3;    // 1:8 prescaler
    T1CONbits.RD16 = 1;      // 16-bit read/write mode
    
    // Set Timer1 reload value for 1ms
    TMR1H = (uint8_t)(TIMER1_RELOAD_VALUE >> 8);
    TMR1L = (uint8_t)(TIMER1_RELOAD_VALUE & 0xFF);
    
    // Enable Timer1 interrupt
    PIE1bits.TMR1IE = 1;     // Enable Timer1 interrupt
    PIR1bits.TMR1IF = 0;     // Clear Timer1 interrupt flag
    
    // Initialize variables
    tick_count = 0;
    timer_running = false;
    registered_callback = NULL;
    
    return TIMER_STATUS_OK;
}

/**
 * @brief Start timer
 */
timer_status_t timer_start(void) {
    if (timer_running) {
        return TIMER_STATUS_RUNNING;
    }
    
    // Reset timer
    TMR1H = (uint8_t)(TIMER1_RELOAD_VALUE >> 8);
    TMR1L = (uint8_t)(TIMER1_RELOAD_VALUE & 0xFF);
    
    // Clear interrupt flag
    PIR1bits.TMR1IF = 0;
    
    // Start Timer1
    T1CONbits.TMR1ON = 1;
    timer_running = true;
    
    return TIMER_STATUS_OK;
}

/**
 * @brief Stop timer
 */
timer_status_t timer_stop(void) {
    if (!timer_running) {
        return TIMER_STATUS_STOPPED;
    }
    
    // Stop Timer1
    T1CONbits.TMR1ON = 0;
    timer_running = false;
    
    return TIMER_STATUS_OK;
}

/**
 * @brief Check if timer is running
 */
bool timer_is_running(void) {
    return timer_running;
}

/**
 * @brief Set timer period
 */
timer_status_t timer_set_period(uint16_t period_ms) {
    if (period_ms == 0) {
        return TIMER_STATUS_ERROR;
    }
    
    timer_period_ms = period_ms;
    return TIMER_STATUS_OK;
}

/**
 * @brief Get current timer period
 */
uint16_t timer_get_period(void) {
    return timer_period_ms;
}

/**
 * @brief Register callback function for timer overflow
 */
void timer_register_callback(timer_callback_t callback) {
    registered_callback = callback;
}

/**
 * @brief Timer interrupt service routine
 */
void timer_isr(void) {
    // Check if Timer1 interrupt occurred
    if (PIR1bits.TMR1IF == 1) {
        // Clear interrupt flag
        PIR1bits.TMR1IF = 0;
        
        // Reload timer for next 1ms
        TMR1H = (uint8_t)(TIMER1_RELOAD_VALUE >> 8);
        TMR1L = (uint8_t)(TIMER1_RELOAD_VALUE & 0xFF);
        
        // Increment tick count
        tick_count++;
        
        // Check if it's time to call the periodic task
        if ((tick_count % timer_period_ms) == 0) {
            timer_periodic_task();
        }
    }
}

/**
 * @brief Get timer tick count
 */
uint32_t timer_get_tick_count(void) {
    return tick_count;
}

/**
 * @brief Reset timer tick count
 */
void timer_reset_tick_count(void) {
    tick_count = 0;
}

/**
 * @brief Enable timer interrupts
 */
void timer_enable_interrupts(void) {
    PIE1bits.TMR1IE = 1;
}

/**
 * @brief Disable timer interrupts
 */
void timer_disable_interrupts(void) {
    PIE1bits.TMR1IE = 0;
}

/**
 * @brief Simple delay function using timer
 */
void timer_delay_ms(uint16_t delay_ms) {
    uint32_t start_time = tick_count;
    
    while ((tick_count - start_time) < delay_ms) {
        // Wait for delay to complete
        // This is a blocking delay
    }
}

/**
 * @brief Check if specified time has elapsed
 */
bool timer_elapsed(uint32_t last_time, uint16_t interval_ms) {
    return ((tick_count - last_time) >= interval_ms);
}

/**
 * @brief Periodic task handler
 */
void timer_periodic_task(void) {
    // Call registered callback if available
    if (registered_callback != NULL) {
        registered_callback();
    }
    
    // Add any other periodic tasks here
    // For example: periodic CAN message transmission
}

/**
 * @brief Example periodic callback function
 * This demonstrates how to use the timer for periodic CAN transmission
 */
void example_periodic_callback(void) {
    // This function could be registered as a callback
    // to perform periodic tasks like sending status messages
    
    // Example: Send a periodic status message
    /*
    static uint8_t status_counter = 0;
    can_message_t status_msg;
    
    status_msg.id = 0x300;  // Status message ID
    status_msg.length = 2;
    status_msg.data[0] = 0x01;  // Status type
    status_msg.data[1] = status_counter++;
    status_msg.valid = true;
    
    can_send_message(&status_msg);
    */
}