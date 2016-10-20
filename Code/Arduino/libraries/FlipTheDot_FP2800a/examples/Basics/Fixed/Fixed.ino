/*
  Fixed
  Iterate thru the output numbers from 1 to 28 and both data pin states, repeatedly.

  This example utilizes two FP2800a ICs where one is used todo the reset action and the other one the set action.
  Both share the same output selections pins (A0, A1, A2, B0 and B1) on the microcontroller but have fixed data pins
  (first LOW and second HIGH) and individual enable pins.
  Instead of toggling the data pin state when needed, this implementation change the enable pin to enable the first 
  or the second FP2800a ICs.

  More details are available in the example "Basic" and the definition of class FlipTheDot_FP2800a.

  This example code is in the public domain.

  modified 20 October 2016
  by Robert Römer
 */


// uncomment these lines to get some debug informations via the defined Serial connection
/*
#ifndef FlipTheDot_FP2800aFixed_DEBUG_SERIAL
#define FlipTheDot_FP2800aFixed_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800aFixed.h"


// defining the Arduino pins which control the two FP2800a ICs
const int fp2800a_pin_ENABLE_RESET  = A0; // enable of the first IC which does the reset
const int fp2800a_pin_ENABLE_SET    = A1; // enable of the second IC which does the set

const int fp2800a_pin_A0        = 2; // D2
const int fp2800a_pin_A1        = 3; // D3
const int fp2800a_pin_A2        = 4; // D4
const int fp2800a_pin_B0        = 5; // D5
const int fp2800a_pin_B1        = 6; // D6

const int fp2800a_pulse_length  = 100; // microseconds


// setup the FP2800a controller object
FlipTheDot_FP2800aFixed controller(
                            fp2800a_pin_ENABLE_RESET,
                            fp2800a_pin_ENABLE_SET,
                            fp2800a_pin_A0,
                            fp2800a_pin_A1,
                            fp2800a_pin_A2,
                            fp2800a_pin_B0,
                            fp2800a_pin_B1,
                            fp2800a_pulse_length
);


// helper variables
int outputNo = 0;
boolean dataStatus = false;



void setup() {
  // initialize debug Serial connection if defined
  #ifdef FlipTheDot_FP2800aFixed_DEBUG_SERIAL
  FlipTheDot_FP2800aFixed_DEBUG_SERIAL.begin(9600);
  #endif

  delay(1000);
  controller.setData( !dataStatus );
}


void loop() {
  if ( outputNo < 0 || outputNo >= controller.getOutputMax() )
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
