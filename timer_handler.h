/**
 * @file timer_handler.h
 * @author oruc
 * @date 2025-07-09
 * @brief Timer handler header for periodic operations
 */

#ifndef TIMER_HANDLER_H
#define TIMER_HANDLER_H

#include "config.h"

// Timer Status
typedef enum {
    TIMER_STATUS_OK,
    TIMER_STATUS_ERROR,
    TIMER_STATUS_RUNNING,
    TIMER_STATUS_STOPPED
} timer_status_t;

// Timer Callback Function Type
typedef void (*timer_callback_t)(void);

// Function Prototypes

/**
 * @brief Initialize timer module
 * @return Timer status
 */
timer_status_t timer_init(void);

/**
 * @brief Start timer
 * @return Timer status
 */
timer_status_t timer_start(void);

/**
 * @brief Stop timer
 * @return Timer status
 */
timer_status_t timer_stop(void);

/**
 * @brief Check if timer is running
 * @return True if timer is running, false otherwise
 */
bool timer_is_running(void);

/**
 * @brief Set timer period
 * @param period_ms Timer period in milliseconds
 * @return Timer status
 */
timer_status_t timer_set_period(uint16_t period_ms);

/**
 * @brief Get current timer period
 * @return Timer period in milliseconds
 */
uint16_t timer_get_period(void);

/**
 * @brief Register callback function for timer overflow
 * @param callback Pointer to callback function
 */
void timer_register_callback(timer_callback_t callback);

/**
 * @brief Timer interrupt service routine
 * Called from main ISR when timer interrupt occurs
 */
void timer_isr(void);

/**
 * @brief Get timer tick count
 * @return Current tick count
 */
uint32_t timer_get_tick_count(void);

/**
 * @brief Reset timer tick count
 */
void timer_reset_tick_count(void);

/**
 * @brief Enable timer interrupts
 */
void timer_enable_interrupts(void);

/**
 * @brief Disable timer interrupts
 */
void timer_disable_interrupts(void);

/**
 * @brief Simple delay function using timer
 * @param delay_ms Delay in milliseconds
 */
void timer_delay_ms(uint16_t delay_ms);

/**
 * @brief Check if specified time has elapsed
 * @param last_time Previous time reference
 * @param interval_ms Interval to check in milliseconds
 * @return True if interval has elapsed, false otherwise
 */
bool timer_elapsed(uint32_t last_time, uint16_t interval_ms);

/**
 * @brief Periodic task handler
 * Called by timer ISR to handle periodic operations
 */
void timer_periodic_task(void);

#endif // TIMER_HANDLER_H