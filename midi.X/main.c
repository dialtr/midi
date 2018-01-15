/*
 * File:   main.c
 * Author: tdial
 *
 * Created on January 15, 2018, 4:41 AM
 */


#include <xc.h>
#include <pic18f4620.h>
#include <delays.h>
#include <plib.h>
#include <usart.h>

// We have to define the clock frequency in Mhz for library functions.
#define _XTAL_FREQ 8000000

// The OSC parameter sets the internal clock up.
#pragma config OSC = HS
#pragma config MCLRE = OFF
#pragma config PWRT = OFF
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config DEBUG = OFF
#pragma config LVP = OFF
#pragma config BOREN = 0
//#pragma config ANSELC = 0


// Initialize UART
//  https://electrosome.com/uart-pic-microcontroller-mplab-xc8/
char uart_init(const long int baudrate) {
    unsigned int x;
    x = (_XTAL_FREQ - (baudrate * 64)) / (baudrate * 64);
    if (x > 255) {
        x = (_XTAL_FREQ - (baudrate * 16)) / (baudrate * 16);
        BRGH = 1;
    }
    
    if (x < 256) {
        SPBRG = x;
        SYNC = 0;
        SPEN = 1;
        TRISC7 = 1;
        TRISC6 = 1;
        CREN = 1;
        TXEN = 1;
        return 1;
    }
    return 0;
}


// Return 1 if the UART has a byte to read.
char uart_data_ready() {
    return RCIF;
}


// Read a single byte from the UART
//   https://electrosome.com/uart-pic-microcontroller-mplab-xc8/
char uart_read() {
    while (!RCIF);
    return RCREG;
}


char port_init() {
    TRISD = 0;
    PORTD = 0;
    return 1;
}

void error();

char init() {
    if (!port_init()) {
        error();
    }
    if (!uart_init(31250)) {
        error();
    }
}

void blink() {
    PORTDbits.RD1 = 1;
    __delay_ms(1);
    PORTDbits.RD1 = 0;
}

void wait() {
    __delay_ms(1000);
}

void error() {
    for (;;) {
        blink();
        blink();
        blink();
        wait();
    }
}

void loop() {
    char b = 0;
    if (uart_data_ready()) {
        b = uart_read();
        blink();
    }
}

void main(void) {
    init();
    for (;;) {
        loop();
    }
    return;
}
