/*
 * codegen.cc
 *
 * Copyright (C) 2018 Thomas R. Dial
 * All Rights Reserved
 *
 * The purpose of this program is to generate C/C++ code for an integer
 * table that represents the frequency in Hertz for a given MIDI note
 * number. The index into the generated array is the MIDI note number,
 * and the value at that index is the frequency in Hertz rounded to the
 * the nearest integer value.
 */

#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]) {
  // Reference value for A above middle C.
  const double A440 = 440;

  // There are 128 MIDI notes.
  const unsigned int FREQ_TABLE_SIZE = 128;  

  // Declare a fixed-size array to hold the frequencies for each MIDI note.
  double frequency_table[FREQ_TABLE_SIZE] = {0};

  // Generate the frequency table.
  for (int i = 0; i < FREQ_TABLE_SIZE; ++i) {
    const double x = i;
    const double f = (A440 / 32.0f) * pow(2.0, ((x - 9.0f) / 12.0f));
    frequency_table[i] = f;
  }

  // Print prefix and first element
  printf("const int MIDI_NOTE_FREQUENCY_TABLE[] = {\n");
  printf("    %d", (int)frequency_table[0]); 

  // Print all entries in the frequency table.
  for (int i = 1; i < FREQ_TABLE_SIZE; ++i) {
    printf(",\n");
    printf("    %d", (int)round(frequency_table[i]));
  }

  // Print closing bracket.
  printf("\n};\n");

  return 0;
}
