#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// Configuration bits for PIC18F2480
#pragma config OSC = HS         // High Speed Crystal/Resonator
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF       // Internal External Switchover bit disabled
#pragma config PWRT = OFF       // Power-up Timer disabled
#pragma config BOREN = BOHW     // Brown-out Reset enabled in hardware
#pragma config BORV = 3         // Brown-out Reset voltage
#pragma config WDT = OFF        // Watchdog Timer disabled
#pragma config WDTPS = 32768    // Watchdog Timer Postscale
#pragma config MCLRE = ON       // MCLR pin enabled
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator disabled
#pragma config PBADEN = OFF     // Port B A/D Enable disabled
#pragma config STVREN = ON      // Stack Full/Underflow will cause Reset
#pragma config LVP = OFF        // Low Voltage Programming disabled
#pragma config XINST = OFF      // Instruction set extension disabled
#pragma config CP0 = OFF        // Code protection disabled
#pragma config CP1 = OFF        // Code protection disabled
#pragma config CPB = OFF        // Boot Block code protection disabled
#pragma config CPD = OFF        // Data EEPROM code protection disabled
#pragma config WRT0 = OFF       // Write protection disabled
#pragma config WRT1 = OFF       // Write protection disabled
#pragma config WRTB = OFF       // Boot Block write protection disabled
#pragma config WRTC = OFF       // Configuration Register write protection disabled
#pragma config WRTD = OFF       // Data EEPROM write protection disabled
#pragma config EBTR0 = OFF      // Table Read protection disabled
#pragma config EBTR1 = OFF      // Table Read protection disabled
#pragma config EBTRB = OFF      // Boot Block Table Read protection disabled

// System definitions
#define _XTAL_FREQ 20000000     // 20MHz crystal frequency
#define FCY (_XTAL_FREQ/4)      // Instruction cycle frequency

// CAN Bus definitions
#define CAN_BAUDRATE 250000     // 250kbps CAN bus speed
#define DATA_MSG_ID 0x100       // Incoming data message ID
#define MASTER_REQ_ID 0x200     // Master request message ID

// Data array size
#define MAX_DATA_SIZE 8         // Maximum CAN message data size
#define DATA_ARRAY_SIZE 16      // Size of data storage array

#endif // CONFIG_H