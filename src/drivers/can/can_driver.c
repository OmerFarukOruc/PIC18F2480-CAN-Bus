#include "can_driver.h"
#include "common/config.h"
#include "app/data_manager.h"

void can_init(void) {
    // Enter Configuration Mode
    CANCON = 0x80;
    while((CANSTAT & 0xE0) != 0x80);

    // Configure CAN bus timing
    // Baud Rate = FCY / (2 * N * BRP) where N = SJW + PRSEG + SEG1PH + SEG2PH
    // Baud Rate = 20MHz / (4 * 2 * (1 + 3 + 3 + 1) * (BRP+1))
    // Baud Rate = 2.5MHz / (8 * (BRP+1))
    // For 250kbps, BRP should be 9
    BRGCON1 = 0x09; // BRP = 9, SJW = 1
    BRGCON2 = 0x9A; // SAM = 1, SEG2PHTS = 1, SEG1PH = 3, PRSEG = 2
    BRGCON3 = 0x02; // WAKFIL = 0, WAKDIS = 0, SEG2PH = 3

    // Set up receive filters and masks
    // For now, accept all messages
    RXM0SIDH = 0x00;
    RXM0SIDL = 0x00;
    RXM1SIDH = 0x00;
    RXM1SIDL = 0x00;

    // Configure receive buffers to accept all valid messages
    RXB0CON = 0x60;
    RXB1CON = 0x60;

    // Return to Normal Mode
    CANCON = 0x00;
    while((CANSTAT & 0xE0) != 0x00);

    // Enable CAN interrupts
    PIE3bits.RXB0IE = 1;
    PIE3bits.RXB1IE = 1;
    IPR3bits.RXB0IP = 1; // High priority
    IPR3bits.RXB1IP = 1; // High priority
}

bool can_transmit(can_message_t *msg) {
    if (TXB0CONbits.TXREQ) {
        return false; // Transmission buffer is busy
    }

    // Load message ID
    if (msg->extended) {
        // Extended ID
        TXB0SIDH = (uint8_t)(msg->id >> 21);
        TXB0SIDL = (uint8_t)(((msg->id >> 13) & 0xE0) | 0x08 | ((msg->id >> 16) & 0x03));
        TXB0EIDH = (uint8_t)(msg->id >> 8);
        TXB0EIDL = (uint8_t)(msg->id);
    } else {
        // Standard ID
        TXB0SIDH = (uint8_t)(msg->id >> 3);
        TXB0SIDL = (uint8_t)(msg->id << 5);
    }

    // Load data length and data
    TXB0DLC = msg->length;
    for (uint8_t i = 0; i < msg->length; i++) {
        *(&TXB0D0 + i) = msg->data[i];
    }

    // Request transmission
    TXB0CONbits.TXREQ = 1;
    return true;
}



void can_interrupt_handler(void) {
    if (PIR3bits.RXB0IF) {
        can_message_t msg;
        // Directly process the message from RXB0
        if (RXB0SIDLbits.EXID) {
            msg.id = ((uint32_t)RXB0SIDH << 21) | ((uint32_t)(RXB0SIDL & 0xE0) << 13) | ((uint32_t)(RXB0SIDL & 0x03) << 16) | ((uint32_t)RXB0EIDH << 8) | (uint32_t)RXB0EIDL;
            msg.extended = true;
        } else {
            msg.id = ((uint32_t)RXB0SIDH << 3) | (RXB0SIDL >> 5);
            msg.extended = false;
        }
        msg.length = RXB0DLC & 0x0F;
        for (uint8_t i = 0; i < msg.length; i++) {
            msg.data[i] = *(&RXB0D0 + i);
        }
        RXB0CONbits.RXFUL = 0;
        data_manager_process_message(&msg);
        PIR3bits.RXB0IF = 0;
    }

    if (PIR3bits.RXB1IF) {
        can_message_t msg;
        // Directly process the message from RXB1
        if (RXB1SIDLbits.EXID) {
            msg.id = ((uint32_t)RXB1SIDH << 21) | ((uint32_t)(RXB1SIDL & 0xE0) << 13) | ((uint32_t)(RXB1SIDL & 0x03) << 16) | ((uint32_t)RXB1EIDH << 8) | (uint32_t)RXB1EIDL;
            msg.extended = true;
        } else {
            msg.id = ((uint32_t)RXB1SIDH << 3) | (RXB1SIDL >> 5);
            msg.extended = false;
        }
        msg.length = RXB1DLC & 0x0F;
        for (uint8_t i = 0; i < msg.length; i++) {
            msg.data[i] = *(&RXB1D0 + i);
        }
        RXB1CONbits.RXFUL = 0;
        data_manager_process_message(&msg);
        PIR3bits.RXB1IF = 0;
    }
}


