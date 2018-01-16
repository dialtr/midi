/*
 * File:   intel_8254.h
 * Author: Thomas R. Dial <dialtr@gmail.com>
 * Date:   15 Jan 2018, 23:07
 */
#ifndef INTEL_8254_H_INCLUDED_
#define INTEL_8254_H_INCLUDED_

// Initialize the Intel 8254 timer to a known state.
char intel_8254_init();

// Load a new divisor into timer zero for square-wave clock generation.
void intel_8254_set_timer0(unsigned char lsb, unsigned char msb);

#endif  // INTEL_8254_H_INCLUDED_
