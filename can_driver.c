/**
 * @file can_driver.c
 * @author oruc
 * @date 2025-07-09
 * @brief CAN bus driver implementation for PIC18F2480 ECAN module
 */

#include "can_driver.h"
#include "data_manager.h"

// Private variables
static volatile bool can_message_received = false;
static volatile bool can_transmission_complete = false;
static volatile uint8_t can_error_count = 0;
static can_mode_t current_mode = CAN_MODE_CONFIGURATION;

// Private function prototypes
static void can_configure_timing(void);
static void can_configure_filters(void);
static can_status_t can_wait_for_mode_change(can_mode_t target_mode);

/**
 * @brief Initialize CAN bus module
 */
can_status_t can_init(void) {
    // Set CAN to configuration mode
    if (can_set_mode(CAN_MODE_CONFIGURATION) != CAN_STATUS_OK) {
        return CAN_STATUS_ERROR;
    }
    
    // Configure CAN I/O pins
    // RB2 = CANTX, RB3 = CANRX for PIC18F2480
    TRISBbits.TRISB2 = 0;  // CANTX output
    TRISBbits.TRISB3 = 1;  // CANRX input
    
    // Configure CAN timing
    can_configure_timing();
    
    // Configure message filters
    can_configure_filters();
    
    // Enable CAN interrupts
    PIE3bits.RXB0IE = 1;   // RX buffer 0 interrupt enable
    PIE3bits.RXB1IE = 1;   // RX buffer 1 interrupt enable
    PIE3bits.TXB0IE = 1;   // TX buffer 0 interrupt enable
    PIE3bits.TXB1IE = 1;   // TX buffer 1 interrupt enable
    PIE3bits.TXB2IE = 1;   // TX buffer 2 interrupt enable
    PIE3bits.ERRIE = 1;    // Error interrupt enable
    
    // Clear interrupt flags
    PIR3bits.RXB0IF = 0;
    PIR3bits.RXB1IF = 0;
    PIR3bits.TXB0IF = 0;
    PIR3bits.TXB1IF = 0;
    PIR3bits.TXB2IF = 0;
    PIR3bits.ERRIF = 0;
    
    // Enable peripheral interrupts
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    // Set to normal mode
    if (can_set_mode(CAN_MODE_NORMAL) != CAN_STATUS_OK) {
        return CAN_STATUS_ERROR;
    }
    
    return CAN_STATUS_OK;
}

/**
 * @brief Configure CAN timing for 125 kbps
 */
static void can_configure_timing(void) {
    // Assuming 20MHz crystal
    // For 125 kbps: BRP = 9, SJW = 1
    // PRSEG = 2, SEG1PH = 7, SEG2PH = 6, SAM = 0
    
    BRGCON1 = 0x09;  // BRP = 9, SJW = 1
    BRGCON2 = 0x9A;  // SAM = 1, SEG2PHTS = 1, SEG1PH = 3, PRSEG = 2
    BRGCON3 = 0x02;  // WAKFIL = 0, WAKDIS = 0, SEG2PH = 3
}

/**
 * @brief Configure CAN message filters
 */
static void can_configure_filters(void) {
    // Configure RX buffer 0 for data messages (0x100)
    RXF0SIDH = (uint8_t)(CAN_DATA_MSG_ID >> 3);
    RXF0SIDL = (uint8_t)(CAN_DATA_MSG_ID << 5);
    
    // Configure RX buffer 1 for request messages (0x200)
    RXF1SIDH = (uint8_t)(CAN_REQUEST_MSG_ID >> 3);
    RXF1SIDL = (uint8_t)(CAN_REQUEST_MSG_ID << 5);
    
    // Configure masks (for now, accept all messages)
    RXM0SIDH = 0x00;
    RXM0SIDL = 0x00;
    RXM1SIDH = 0x00;
    RXM1SIDL = 0x00;
    
    // Configure buffers to receive all valid messages
    RXB0CON = 0x60;  // Receive all valid messages
    RXB1CON = 0x60;  // Receive all valid messages
}

/**
 * @brief Set CAN operation mode
 */
can_status_t can_set_mode(can_mode_t mode) {
    switch (mode) {
        case CAN_MODE_CONFIGURATION:
            CANCON = 0x80;  // Request configuration mode
            break;
        case CAN_MODE_NORMAL:
            CANCON = 0x00;  // Request normal mode
            break;
        case CAN_MODE_LOOPBACK:
            CANCON = 0x40;  // Request loopback mode
            break;
        case CAN_MODE_LISTEN_ONLY:
            CANCON = 0x60;  // Request listen-only mode
            break;
        default:
            return CAN_STATUS_ERROR;
    }
    
    // Wait for mode change
    if (can_wait_for_mode_change(mode) != CAN_STATUS_OK) {
        return CAN_STATUS_TIMEOUT;
    }
    
    current_mode = mode;
    return CAN_STATUS_OK;
}

/**
 * @brief Wait for CAN mode change
 */
static can_status_t can_wait_for_mode_change(can_mode_t target_mode) {
    uint16_t timeout = 1000;
    uint8_t expected_opmode;
    
    switch (target_mode) {
        case CAN_MODE_CONFIGURATION:
            expected_opmode = 0x80;
            break;
        case CAN_MODE_NORMAL:
            expected_opmode = 0x00;
            break;
        case CAN_MODE_LOOPBACK:
            expected_opmode = 0x40;
            break;
        case CAN_MODE_LISTEN_ONLY:
            expected_opmode = 0x60;
            break;
        default:
            return CAN_STATUS_ERROR;
    }
    
    while ((CANSTAT & 0xE0) != expected_opmode && timeout > 0) {
        __delay_us(10);
        timeout--;
    }
    
    return (timeout > 0) ? CAN_STATUS_OK : CAN_STATUS_TIMEOUT;
}

/**
 * @brief Get current CAN operation mode
 */
can_mode_t can_get_mode(void) {
    return current_mode;
}

/**
 * @brief Send CAN message
 */
can_status_t can_send_message(const can_message_t* message) {
    if (message == NULL || message->length > 8) {
        return CAN_STATUS_ERROR;
    }
    
    // Check if TX buffer 0 is available
    if (TXB0CONbits.TXREQ == 1) {
        return CAN_STATUS_BUSY;
    }
    
    // Set message ID
    TXB0SIDH = (uint8_t)(message->id >> 3);
    TXB0SIDL = (uint8_t)(message->id << 5);
    
    // Set data length
    TXB0DLC = message->length;
    
    // Copy message data
    TXB0D0 = message->data[0];
    TXB0D1 = message->data[1];
    TXB0D2 = message->data[2];
    TXB0D3 = message->data[3];
    TXB0D4 = message->data[4];
    TXB0D5 = message->data[5];
    TXB0D6 = message->data[6];
    TXB0D7 = message->data[7];
    
    // Clear transmission complete flag
    can_transmission_complete = false;
    
    // Request transmission
    TXB0CONbits.TXREQ = 1;
    
    return CAN_STATUS_OK;
}

/**
 * @brief Check if CAN message is available
 */
bool can_message_available(void) {
    return can_message_received;
}

/**
 * @brief Receive CAN message
 */
can_status_t can_receive_message(can_message_t* message) {
    if (message == NULL) {
        return CAN_STATUS_ERROR;
    }
    
    // Check RX buffer 0
    if (RXB0CONbits.RXFUL == 1) {
        // Read message ID
        message->id = ((uint16_t)RXB0SIDH << 3) | (RXB0SIDL >> 5);
        
        // Read data length
        message->length = RXB0DLC & 0x0F;
        
        // Read message data
        message->data[0] = RXB0D0;
        message->data[1] = RXB0D1;
        message->data[2] = RXB0D2;
        message->data[3] = RXB0D3;
        message->data[4] = RXB0D4;
        message->data[5] = RXB0D5;
        message->data[6] = RXB0D6;
        message->data[7] = RXB0D7;
        
        message->valid = true;
        
        // Clear buffer
        RXB0CONbits.RXFUL = 0;
        can_message_received = false;
        
        return CAN_STATUS_OK;
    }
    
    // Check RX buffer 1
    if (RXB1CONbits.RXFUL == 1) {
        // Read message ID
        message->id = ((uint16_t)RXB1SIDH << 3) | (RXB1SIDL >> 5);
        
        // Read data length
        message->length = RXB1DLC & 0x0F;
        
        // Read message data
        message->data[0] = RXB1D0;
        message->data[1] = RXB1D1;
        message->data[2] = RXB1D2;
        message->data[3] = RXB1D3;
        message->data[4] = RXB1D4;
        message->data[5] = RXB1D5;
        message->data[6] = RXB1D6;
        message->data[7] = RXB1D7;
        
        message->valid = true;
        
        // Clear buffer
        RXB1CONbits.RXFUL = 0;
        can_message_received = false;
        
        return CAN_STATUS_OK;
    }
    
    return CAN_STATUS_ERROR;
}

/**
 * @brief CAN interrupt service routine
 */
void can_isr(void) {
    can_message_t received_message;
    
    // Check RX buffer 0 interrupt
    if (PIR3bits.RXB0IF == 1) {
        PIR3bits.RXB0IF = 0;
        can_message_received = true;
        
        // Process received message
        if (can_receive_message(&received_message) == CAN_STATUS_OK) {
            data_manager_process_message(&received_message);
        }
    }
    
    // Check RX buffer 1 interrupt
    if (PIR3bits.RXB1IF == 1) {
        PIR3bits.RXB1IF = 0;
        can_message_received = true;
        
        // Process received message
        if (can_receive_message(&received_message) == CAN_STATUS_OK) {
            data_manager_process_message(&received_message);
        }
    }
    
    // Check TX buffer interrupts
    if (PIR3bits.TXB0IF == 1) {
        PIR3bits.TXB0IF = 0;
        can_transmission_complete = true;
    }
    
    if (PIR3bits.TXB1IF == 1) {
        PIR3bits.TXB1IF = 0;
        can_transmission_complete = true;
    }
    
    if (PIR3bits.TXB2IF == 1) {
        PIR3bits.TXB2IF = 0;
        can_transmission_complete = true;
    }
    
    // Check error interrupt
    if (PIR3bits.ERRIF == 1) {
        PIR3bits.ERRIF = 0;
        can_error_count++;
        
        // Handle CAN errors
        if (COMSTATbits.TXBO == 1) {
            // Bus-off error - reinitialize CAN
            can_init();
        }
    }
}

/**
 * @brief Enable CAN interrupts
 */
void can_enable_interrupts(void) {
    PIE3bits.RXB0IE = 1;
    PIE3bits.RXB1IE = 1;
    PIE3bits.TXB0IE = 1;
    PIE3bits.TXB1IE = 1;
    PIE3bits.TXB2IE = 1;
    PIE3bits.ERRIE = 1;
}

/**
 * @brief Disable CAN interrupts
 */
void can_disable_interrupts(void) {
    PIE3bits.RXB0IE = 0;
    PIE3bits.RXB1IE = 0;
    PIE3bits.TXB0IE = 0;
    PIE3bits.TXB1IE = 0;
    PIE3bits.TXB2IE = 0;
    PIE3bits.ERRIE = 0;
}

/**
 * @brief Set up CAN message filters
 */
can_status_t can_set_filter(uint8_t filter_id, uint16_t msg_id, uint16_t mask) {
    if (filter_id > 5) {
        return CAN_STATUS_ERROR;
    }
    
    // Set to configuration mode for filter setup
    can_mode_t old_mode = current_mode;
    if (can_set_mode(CAN_MODE_CONFIGURATION) != CAN_STATUS_OK) {
        return CAN_STATUS_ERROR;
    }
    
    // Configure filter based on ID
    switch (filter_id) {
        case 0:
            RXF0SIDH = (uint8_t)(msg_id >> 3);
            RXF0SIDL = (uint8_t)(msg_id << 5);
            RXM0SIDH = (uint8_t)(mask >> 3);
            RXM0SIDL = (uint8_t)(mask << 5);
            break;
        case 1:
            RXF1SIDH = (uint8_t)(msg_id >> 3);
            RXF1SIDL = (uint8_t)(msg_id << 5);
            RXM1SIDH = (uint8_t)(mask >> 3);
            RXM1SIDL = (uint8_t)(mask << 5);
            break;
        // Add more filters as needed
        default:
            can_set_mode(old_mode);
            return CAN_STATUS_ERROR;
    }
    
    // Restore previous mode
    can_set_mode(old_mode);
    return CAN_STATUS_OK;
}

/**
 * @brief Get CAN bus error status
 */
uint8_t can_get_error_status(void) {
    return can_error_count;
}

/**
 * @brief Clear CAN error flags
 */
void can_clear_errors(void) {
    can_error_count = 0;
    PIR3bits.ERRIF = 0;
}