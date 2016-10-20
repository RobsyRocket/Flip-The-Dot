/*
  Multi
  Iterate thru the output numbers from 1 to 28 of multiple ICs and both data pin states, repeatedly.

  More details are available in the example "Default" and the definition of class FlipTheDot_FP2800a.
  
  This example code is in the public domain.

  modified 19 October 2016
  by Robert Römer
 */


// uncomment these lines to get some debug informations via the defined Serial connection
/*
#ifndef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
#define FlipTheDot_FP2800aMulti_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800aMulti.h"


// defining the Arduino pins which control the FP2800a ICs
const int fp2800a_pin_DATA      = A0;

const int fp2800a_pin_ENABLE_1  = A1;
const int fp2800a_pin_ENABLE_2  = A2;
const int fp2800a_pin_ENABLE_3  = A3;
const int fp2800a_pin_ENABLE_4  = A4;
const int fp2800a_pin_ENABLE_5  = A5;

const int fp2800a_pin_A0        = 2; // D2
const int fp2800a_pin_A1        = 3; // D3
const int fp2800a_pin_B0        = 4; // D4
const int fp2800a_pin_B1        = 5; // D5
const int fp2800a_pin_B2        = 6; // D6

const int fp2800a_pulse_length  = 100; // microseconds


// prepare the list of enable pins
int fp2800a_pin_ENABLE_list[] = {
  fp2800a_pin_ENABLE_1,
  fp2800a_pin_ENABLE_2,
  fp2800a_pin_ENABLE_3,
  fp2800a_pin_ENABLE_4,
  fp2800a_pin_ENABLE_5
};


// setup the FP2800aMulti controller object
FlipTheDot_FP2800aMulti controller(
                            fp2800a_pin_ENABLE_list,
                            sizeof(fp2800a_pin_ENABLE_list) / sizeof(int), // list length/size (calculated or static value possible)
                            fp2800a_pin_DATA,
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
  #ifdef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
  FlipTheDot_FP2800aMulti_DEBUG_SERIAL.begin(9600);
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
