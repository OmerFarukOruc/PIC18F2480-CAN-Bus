#include "timer_handler.h"
#include <xc.h>

static volatile uint32_t tick_count = 0;
static volatile bool timer_running = false;

void timer_init(void) {
    T1CONbits.TMR1ON = 0;
    T1CONbits.T1CKPS = 0b11;
    T1CONbits.TMR1CS = 0;
    T1CONbits.T1SYNC = 0;
    
    TMR1H = 0xFD;
    TMR1L = 0x8F;
    
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    tick_count = 0;
    timer_running = false;
}

void timer_start(void) {
    tick_count = 0;
    timer_running = true;
    T1CONbits.TMR1ON = 1;
}





void timer_interrupt_handler(void) {
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
        
        TMR1H = 0xFD;
        TMR1L = 0x8F;
        
        tick_count++;
    }
}

