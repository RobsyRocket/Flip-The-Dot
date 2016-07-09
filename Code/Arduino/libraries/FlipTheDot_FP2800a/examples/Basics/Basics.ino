/*
  Basics
  Iterate thru the output numbers from 1 to 28 and both data pin states, repeatedly.

  The FP2800a has 5 pins to control which one of the 28 output pins should be used,
  plus 1 pin for data (source/sink mode) and 1 pin to enable the output.

  The selection of the output is done in a binary definition, with low and high levels on the 5 pins.
  These pins are label in the datasheet as A0, A1, B0, B1 and B2 – which has nothing todo with the Arduino Pins A0->Ax.
  Only one output can be selected at the same time.

  The "data" pin decides if the output should act as source or sink.
  By setting this pin low, the selected output will be defined as sink or if high as source to send power from VSS to the connected circuit.

  The selected output does nothing until the "enable" pin is set to high.
  This should be done only for a short time, like 100 micoseconds for a flipdot or longer for LEDs.

  This example code is in the public domain.

  modified 8 June 2016
  by Robert Römer
 */


// uncomment these lines to get some debug informations via the defined Serial connection
/*
#ifndef FlipTheDot_FP2800a_DEBUG_SERIAL
#define FlipTheDot_FP2800a_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800a.h"


// defining the Arduino pins which control the FP2800a
const int fp2800a_pin_DATA      = A0;
const int fp2800a_pin_ENABLE    = A1;

const int fp2800a_pin_A0        = 2; // D2
const int fp2800a_pin_A1        = 3; // D3
const int fp2800a_pin_B0        = 4; // D4
const int fp2800a_pin_B1        = 5; // D5
const int fp2800a_pin_B2        = 6; // D6

const int fp2800a_pulse_length  = 100; // microseconds


// setup the FP2800a controller object
FlipTheDot_FP2800a controller(
                            fp2800a_pin_DATA,
                            fp2800a_pin_ENABLE,
                            fp2800a_pin_A0,
                            fp2800a_pin_A1,
                            fp2800a_pin_B0,
                            fp2800a_pin_B1,
                            fp2800a_pin_B2,
                            fp2800a_pulse_length
);


// helper variables
int outputNo = 0;
boolean dataStatus = false;



void setup() {
  // initialize debug Serial connection if defined
  #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
  FlipTheDot_FP2800a_DEBUG_SERIAL.begin(9600);
  #endif

  delay(1000);
  controller.setData( !dataStatus );
}


void loop() {
  if ( outputNo < 0 || outputNo >= 28 )
  {
    // reset output number and invert the data flag
    outputNo = 0;
    dataStatus = !dataStatus;
    controller.setData( dataStatus );
  }

  // increase output number
  outputNo++;

  // set new output number
  controller.setOutput( outputNo );

  // pulse the "enable" pin for the defined time to high and back to low
  controller.pulse();

  delay(500);
}
