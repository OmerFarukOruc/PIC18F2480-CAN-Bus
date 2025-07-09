/**
 * @file config.h
 * @author oruc
 * @date 2025-07-09
 * @brief Configuration file for PIC18F2480 CAN bus project
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// PIC18F2480 Configuration Bits
#pragma config OSC = HS         // High Speed Crystal oscillator
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF       // Internal External Oscillator Switchover disabled
#pragma config PWRT = OFF       // Power-up Timer disabled
#pragma config BOREN = BOHW     // Brown-out Reset enabled in hardware
#pragma config BORV = 3         // Brown-out Reset voltage bits (VBOR set to 2.1V)
#pragma config WDT = OFF        // Watchdog Timer disabled
#pragma config WDTPS = 32768    // Watchdog Timer Postscale bits (1:32768)
#pragma config MCLRE = ON       // MCLR pin enabled; RE3 input pin disabled
#pragma config LPT1OSC = OFF    // Timer1 configured for higher power operation
#pragma config PBADEN = OFF     // Port B A/D Enable disabled
#pragma config STVREN = ON      // Stack full/underflow will cause Reset
#pragma config LVP = OFF        // Single-Supply ICSP disabled
#pragma config XINST = OFF      // Extended Instruction Set disabled
#pragma config CP0 = OFF        // Code Protection bit
#pragma config CP1 = OFF        // Code Protection bit
#pragma config CPB = OFF        // Boot Block Code Protection bit
#pragma config CPD = OFF        // Data EEPROM Code Protection bit
#pragma config WRT0 = OFF       // Write Protection bit
#pragma config WRT1 = OFF       // Write Protection bit
#pragma config WRTB = OFF       // Boot Block Write Protection bit
#pragma config WRTC = OFF       // Configuration Register Write Protection bit
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit
#pragma config EBTR0 = OFF      // Table Read Protection bit
#pragma config EBTR1 = OFF      // Table Read Protection bit
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit

// System Configuration
#define _XTAL_FREQ 20000000     // 20MHz crystal frequency

// CAN Bus Configuration
#define CAN_DATA_MSG_ID     0x100   // Message ID for incoming data messages
#define CAN_REQUEST_MSG_ID  0x200   // Message ID for master request messages
#define CAN_RESPONSE_MSG_ID 0x201   // Message ID for responses to master

// Data Buffer Configuration
#define MAX_DATA_BUFFER_SIZE 8      // Maximum CAN message data length
#define MAX_STORED_MESSAGES 10      // Maximum number of stored CAN messages

// Timer Configuration
#define TIMER_PERIOD_MS 1000        // Timer period in milliseconds (1 second)

// Application States
typedef enum {
    APP_STATE_INIT,
    APP_STATE_RUNNING,
    APP_STATE_ERROR
} app_state_t;

// CAN Message Structure
typedef struct {
    uint16_t id;                    // CAN message ID
    uint8_t length;                 // Data length (0-8)
    uint8_t data[MAX_DATA_BUFFER_SIZE]; // Message data
    bool valid;                     // Message validity flag
} can_message_t;

#endif // CONFIG_H