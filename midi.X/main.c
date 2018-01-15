/*
 * File:   main.c
 * Author: Thomas R. Dial <dialtr@gmail.com>
 * Date:   15 Jan 2018, 4:41 AM
 * 
 * This is an example program that demonstrates how to configure a
 * PIC 18F4620 microcontroller to receive MIDI data using the on-board
 * UART device in the PIC. The program is designed for use Microchip
 * Technologies (TM) XC-8 compiler.
 * 
 * In its present form, the program simply configures the serial port
 * for asynchronous transmit / receive, configures a PORTD pin for output,
 * and then goes into an endless loop waiting for MIDI data. When a
 * MIDI byte is received, the program pulses the LED on PORTD.
 * 
 * If the program encounters an error, it will enter an endless loop in
 * which it will flash 3 times quickly, wait for a second, and repeat.
 * Normal function can be detected as follows:
 * 
 *   * If no MIDI device is detected, the indicator LED will remain dark.
 *   
 *   * If a MIDI device is detected, the indicator will blink periodically
 *     as a result of MIDI status data. Pressing keys or activating
 *     controllers on an attached MIDI controller will result in the LED
 *     lighting for each byte. This happens so quickly if a lot of MIDI
 *     data arrives that it appears that the LED is getting brighter.
 */


#include <xc.h>
#include <pic18f4620.h>
#include <delays.h>
#include <plib.h>
#include <usart.h>

// Here, we are configuring various settings on the PIC. The most important
// setting to note for this example is 'OSC', which we set to 'HS'. This
// configures the PIC to use an external, high-speed crystal oscillator.
// The frequency of the oscillator used for this experiment is 8 MHZ; we must
// define this value below in _XTAL_FREQ as this symbol is used by the UART
// initialization routine below.

#pragma config OSC = HS
#pragma config MCLRE = OFF
#pragma config PWRT = OFF
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config DEBUG = OFF
#pragma config LVP = OFF
#pragma config BOREN = 0

// We have to define the clock frequency in MHZ for library functions.
#define _XTAL_FREQ 8000000


//
// char uart_init(long baudrate)
//
// Initialize the UART for serial data communication using the specified baud
// rate. The function enables transmit AND receive in asynchronous mode, which
// is probably what most people want for experimentation. Depending on the
// speed of the crystal, we must configure registers differently, which is why
// there is logic to calculate whether the crystal / desired baud rate
// combination requires us to set BRGH.
//
// Returns 1 on success. Returns zero on failure.
//
// See Also:
// 
//  https://electrosome.com/uart-pic-microcontroller-mplab-xc8/
//  https://www.mepits.com/tutorial/431/PIC/Introduction-to-PIC16F877
//
// Notes:
//
//  TODO(tdial): Complete explanation of the baud rate calculation and how
//  that translates to values that are loaded into the registers.
//
char uart_init(long int baudrate) {
    unsigned int x;
    x = (_XTAL_FREQ - (baudrate * 64)) / (baudrate * 64);
    if (x > 255) {
        x = (_XTAL_FREQ - (baudrate * 16)) / (baudrate * 16);
        BRGH = 1;
    }
    
    if (x < 256) {
        SPBRG = x;
        SYNC = 0;    // Asynchronous mode (clear sync bit to zero.)
        SPEN = 1;    // Enable serial communication.
        TRISC7 = 1;  // Configure as RX pin (serial data receive)
        TRISC6 = 1;  // Configure as TX pin (serial data transmit)
        CREN = 1;    // Enable reception
        TXEN = 1;    // Enable transmission
        return 1;    // Return success
    }
    return 0;        // Return failure
}


//
// char uart_data_ready()
//
// Poll the UART to see if data is available for receiving. This function
// does *not* block; it returns immediately with a value indicating whether
// there is data in the receive buffer. 
//
// Returns 1 if a byte is available. Returns zero if not.
//
char uart_data_ready() {
    return RCIF;
}


//
// char uart_read()
// 
// Read a single byte from the UART. This function is designed to be used in
// conjunction with uart_data_ready(), which determines whether there is
// actually data to read.
//
// This function "spins" until the RCIF register is set, indicating available
// data. The byte is then returned from RCREG. 
//
// See Also:
//
//   https://electrosome.com/uart-pic-microcontroller-mplab-xc8/
//
// Notes:
//
//   TODO(tdial): Presumably, reading from RCREG clears RCIF. Ensure that
//   this is the case and document it in the code so that readers new to
//   this topic can understand what's going on.
//
char uart_read() {
    while (!RCIF);
    return RCREG;
}


//
// char port_init()
//
// Initialize other ports used in this example.
//
// Returns 1 on success. Returns 0 on failure. It is not really possible
// for this function to fail, but in accordance with the conventions
// established in this example, all initialization routines will return
// 1 on success and 0 on failure.
//
char port_init() {
    // Configure PORTD as a regular output.
    TRISD = 0;
    
    // Clear PORTD pins to zero.
    PORTD = 0;
    
    // All initialization routines will return 1 on success and zero on
    // failure. This routine can't fail, so we will return 1 in all cases.
    return 1;
}


// Declare an error routine; this spins forever and flashes 3 times quickly
// followed by a one second delay, forever. This routine will have to do
// until we have the 16x2 display up and running.
void error();


// Initialize the system (all ports, configure devices, etc.)
char system_init(long baudrate) {
    char status = 0;
    
    status = port_init();
    if (!status) {
        return status;
    }
    
    status = uart_init(baudrate);
    if (!status) {
        return status;
    }
 
    return 1;
}

// Blink a LED (quickly) that is attached to RD1 (PORTD, pin 1.)
void blink() {
    PORTDbits.RD1 = 1;
    __delay_ms(1);
    PORTDbits.RD1 = 0;
}

// Wait for one second.
void wait() {
    __delay_ms(1000);
}


// Routine to display a blinking sequence that indicates error.
void error() {
    for (;;) {
        blink();
        blink();
        blink();
        wait();
    }
}


// Loop forever checking to see if there is data ready from the UART, and if
// so, read the byte and blink the LED that's attached to PORTD pin 1.
void loop() {
    char b = 0;
    if (uart_data_ready()) {
        b = uart_read();
        // In a real system, we probably would not want to call the blink()
        // routine for every byte received. That's because it uses a one ms.
        // delay internally, which could affect performance if the incoming
        // data was arriving at a fast rate. It is nonetheless left here in
        // the example for now.
        blink();
    }
}


// Main program: Perform one-time initialization and then enter a program loop.
void main(void) {
    const long MIDI_BAUD_RATE = 31250;
    
    system_init(MIDI_BAUD_RATE);
    
    for (;;) {
        loop();
    }
    
    return;
}
