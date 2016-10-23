/*
  Fixed-Default Controller
  Iterate thru the output numbers of multiple controllers in a row and column configuration.
  
  This example utilizes three FP2800a ICs where one is used to control the columns and the rest for the rows.
  One row IC does the reset action and the other one the set action. This is needed, because the wiring of a 
  flipdot module consist of three input/output lines and diodes. One line is controlled with the column IC and
  the other two lines are used to source or sink current with the row controllers to set and reset the dots.
  The row ICs share the same output selections pins (A0, A1, A2, B0 and B1) on the microcontroller but have 
  fixed data pins (first LOW, second HIGH) and individual enable pins. Instead of toggling the data pin state 
  when needed, this implementation change the enable pin to enable the first or the second FP2800a ICs.  


  Schematic for the wiring of a single flipdot:
  
          coil      ┌──────< SET_ROW    (goes to one output of the IC with fixed HIGH data line)
            |      _│_
            V      \│/    <-- diode
                   ─┼─
        ┌┐┌┐┌┐┌┐    │
   ┌────┘└┘└┘└┘└────●
   │               _│_
   ^               \│/
  COL              ─┼─      
                    └──────< RESET_ROW  (goes to output of the IC with fixed LOW data line)


  This example code is in the public domain.

  modified 23 October 2016
  by Robert Römer
 */


// uncomment these lines to get some debug informations via the defined Serial connection
/*
#ifndef FlipTheDot_FP2800a_DEBUG_SERIAL
#define FlipTheDot_FP2800a_DEBUG_SERIAL Serial
#endif
#ifndef FlipTheDot_FP2800aFixed_DEBUG_SERIAL
#define FlipTheDot_FP2800aFixed_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800a.h"
#include "FlipTheDot_FP2800aFixed.h"


// defining the pulse length for the FP2800a enable pins
const int fp2800a_pulse_length  = 100; // microseconds

// setup the objects for the fixed FP2800a rows controller and FP2800aMulti columns controller
// Parameter order:                      Enable Reset, Enable Set,  A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800aFixed rowController(   A0,           2,           3,  4,  5,  6,  7,   fp2800a_pulse_length);
// Parameter order:                      Enable,       Data,        A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800a columnController(     A1,           8,           9,  10, 11, 12, 13,  fp2800a_pulse_length);


// helper variables
int rowOutputNo = 0;
int rowOutputNoMax = rowController.getOutputMax();

int columnOutputNo = 0;
int columnOutputNoMax = columnController.getOutputMax();

boolean dataStatus = false;


void setup() {
  // initialize debug Serial connection if defined
  #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
  FlipTheDot_FP2800a_DEBUG_SERIAL.begin(9600);
  #endif
  #ifdef FlipTheDot_FP2800aFixed_DEBUG_SERIAL
  FlipTheDot_FP2800aFixed_DEBUG_SERIAL.begin(9600);
  #endif

  delay(1000);
  rowController.setData( dataStatus );
  columnController.setData( !dataStatus );
}


void loop() {
  // iterate thru all rows
  for ( rowOutputNo = 1; rowOutputNo <= rowOutputNoMax; rowOutputNo++ )
  {
    // update row controller to change row number
    rowController.setOutput(rowOutputNo);
    
    // iterate thru all columns in the current row
    for ( columnOutputNo = 1; columnOutputNo <= columnOutputNoMax; columnOutputNo++ )
    {
      // update columns controller to change column number
      columnController.setOutput(columnOutputNo);
      
      // manual pulse the "enable" pins for the defined time to high and back to low
      // the pulse method connot be used because both enable pins need to be pulsed simultaneously
      rowController.enable();
      columnController.enable();
      delayMicroseconds(fp2800a_pulse_length);
      rowController.disable();
      columnController.disable();

      // remove/decrease this delay to speedup the process
      delay(100);
    }
  }

  // invert data flag and update the controllers
  dataStatus = !dataStatus;
  rowController.setData( dataStatus );
  columnController.setData( !dataStatus );
}
