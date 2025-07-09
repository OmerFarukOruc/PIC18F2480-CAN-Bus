#include "common/config.h"
#include "drivers/can/can_driver.h"
#include "data_manager.h"
#include "drivers/timer/timer_handler.h"
#include <xc.h>

void user_data_callback(uint8_t *data, uint8_t length) {
    // This function is called when a CAN message with ID 0x100 is received.
    // User application logic to process the data should be implemented here.
}

void __interrupt() ISR(void) {
    can_interrupt_handler();
    timer_interrupt_handler();
}

void system_init(void) {
    data_manager_init();
    can_init();
    timer_init();
    
    data_manager_set_user_callback(user_data_callback);
    
    timer_start();
}

int main(void) {
    system_init();
    
    while (1) {
        // All processing is done in interrupts
    }
    
    return 0;
}