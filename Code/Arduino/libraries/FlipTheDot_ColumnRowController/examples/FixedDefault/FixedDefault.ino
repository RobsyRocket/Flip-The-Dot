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


  A almost identical code can be found in the example Controlling_Rows_and_Columns/FixedDefault of the 
  FlipTheDot_FP2800a library. The difference here is that a lot of manual work (like output selection of 
  rows/columns, data state and pulsing) is now wrapped into one object for easier usage.


  This example code is in the public domain.

  modified 9 November 2016
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
#ifndef FlipTheDot_ColumnRowController_DEBUG_SERIAL
#define FlipTheDot_ColumnRowController_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800a.h"
#include "FlipTheDot_FP2800aFixed.h"
#include "FlipTheDot_ColumnRowController.h"


// defining the pulse length for the FP2800a enable pins
const int fp2800a_pulse_length  = 100; // microseconds

// setup the objects for the fixed FP2800a rows controller and FP2800a columns controller
// Parameter order:                      Enable Reset, Enable Set,  A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800aFixed rowController(   A0,           2,           3,  4,  5,  6,  7,   fp2800a_pulse_length);
// Parameter order:                      Enable,       Data,        A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800a columnController(     A1,           8,           9,  10, 11, 12, 13,  fp2800a_pulse_length);


// wrap independent column and row controllers in one object for easier usage
FlipTheDot_ColumnRowController controller(columnController, rowController, fp2800a_pulse_length);


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
  #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
  FlipTheDot_ColumnRowController_DEBUG_SERIAL.begin(9600);
  #endif

  delay(1000);
}


void loop() {
  // iterate thru all rows
  for ( rowOutputNo = 1; rowOutputNo <= rowOutputNoMax; rowOutputNo++ )
  {
    // iterate thru all columns in the current row
    for ( columnOutputNo = 1; columnOutputNo <= columnOutputNoMax; columnOutputNo++ )
    {
      // provide column and row information plus the desired action to the wrapping controller
      // which will update both internal controllers (columns and rows) with the correct setting 
      // (output number and data status) and executes the pulse logic
      controller.flip(columnOutputNo, rowOutputNo, dataStatus);

      // remove/decrease this delay to speedup the process
      delay(1000);
    }
  }

  // invert data flag
  dataStatus = !dataStatus;
}
