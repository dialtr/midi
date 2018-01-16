/*
 * File:   intel_8254.c
 * Author: tdial
 *
 * Created on January 15, 2018, 11:06 PM
 */

#include <xc.h>
#include "intel_8254.h"

#define NOPWAIT() Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();\
                  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();\
                  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();\
                  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop()


// Initialize the Intel 8254 timer to a known state.
char intel_8254_init() {
    // Port B is output
    TRISB = 0;
    
    // Port D is output
    TRISD = 0;
    
    // Initialize the 8254 to a known state.  CS, WR are high,
    // meaning that the chip is not selected or write enabled.
    // A0, A1 are high, meaning that if a word was written it
    // would go to the control register.
    PORTDbits.RD2 = 1; // RD2 on the PIC -> CS on the 8254
    PORTDbits.RD3 = 1; // RD3 on the PIC -> WR on the 8254
    PORTDbits.RD4 = 1; // RD4 on the PIC -> A0 on the 8254
    PORTDbits.RD5 = 1; // RD5 on the PIC -> A1 on the 8254
    
    // Initial Port B lines are zero.
    PORTB = 0;
    
    return 1;
}

// Assert the lines in the correct order to transfer a byte to the 8254.
// TODO(tdial): There are some underlying assumptions here concerning
// the clock speed of the PIC18. The NOPWAIT() macro is configured to
// work with an 8 Mhz master clock.  Need to figure out a way to wait
// minimally based on _XTAL_FREQ to make transfers as fast as possible.

void intel_8254_transfer_data() {
  // Bring CS, WR LOW, then HIGH to write control word
  // TODO: Break this out into a function
  PORTDbits.RD2 = 0;  // CS -> LOW
  NOPWAIT();          
  
  PORTDbits.RD3 = 0;  // WR -> LOW
  NOPWAIT();
  NOPWAIT();
  
  PORTDbits.RD3 = 1;  // WR -> HIGH
  NOPWAIT();
  
  PORTDbits.RD2 = 1;  // CS -> HIGH
  NOPWAIT();
}

// Load a new divisor into timer zero for square-wave clock generation.
void intel_8254_set_timer0(unsigned char lsb, unsigned char msb) {
    const unsigned char CONTROL_WORD = 0b00110110;
        
  // Step 0: Bring timer pins to known, initialized state.
  PORTDbits.RD2 = 1;
  PORTDbits.RD3 = 1;
  NOPWAIT();
  
  // Step 1:  Write control word. First, we set up A0, A1 to address
  // the control register (both HIGH.) Next, we assert the data pins
  // with the control word. Finally, we assert (CS, WR) for write.
  PORTDbits.RD4 = 1;
  PORTDbits.RD5 = 1;
  PORTB = CONTROL_WORD;
  
  // Prior to enabling CS, WR, ensure lines are "stable" (NOTE: not sure 
  // if this is necessary or not.)
  NOPWAIT();
  
  // Bring CS, WR LOW, then HIGH to write control word
  intel_8254_transfer_data();
  
  // Step 2: Write LSB. First, we set A0, A1 to address Timer 0 (both set
  // LOW.) Next, we assert the data pins with the LSB. Finally, we assert
  // CS, WR as we did above.
  PORTDbits.RD4 = 0;
  PORTDbits.RD5 = 0;
  PORTB = lsb;
  
  // Ensure lines are "stable" before enabling CS, WR.  
  NOPWAIT(); 
  
  // Bring CS, WR LOW, then HIGH to write control word
  intel_8254_transfer_data();
  
  // Step 3: Write MSB. First, we set A0, A1 to address Timer 0 (both set
  // LOW.) Next, we assert the data pins with the MSB. Finally, we assert
  // CS, WR as we did above.
  PORTDbits.RD4 = 0;
  PORTDbits.RD5 = 0;
  PORTB = msb;
  
  // Ensure lines are "stable" before enabling CS, WR.  
  NOPWAIT(); 
  
  // Bring CS, WR LOW, then HIGH to write control word
  intel_8254_transfer_data();
}

