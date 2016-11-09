/*
  Default-Default Controller
  Iterate thru the output numbers of two controllers in a row and column configuration.
  
  This script utilize two FP2800a ICs to control a single flipdot matrix. The column controller gets 
  used as is and span across 28 columns. The wiring of a single flipdot require the use of two control
  lines for every row. To align with this requirement, the outputs of the row IC get splitted into 
  two groups in this example - first half for the RESET action and the second half for SET.
  

  Schematic for the wiring of a single flipdot:
  
          coil      ┌──────< SET_ROW    (goes to one output of the SECOND half)
           |       _│_
           V       \│/    <-- diode
                   ─┼─
        ┌┐┌┐┌┐┌┐    │
   ┌────┘└┘└┘└┘└────●
   │               _│_
   │               \│/
   ^               ─┼─
  COL               └──────< RESET_ROW  (goes to one output of the FIRST half)


  This example code is in the public domain.

  modified 8 November 2016
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


// defining the pulse length for the FP2800a enable pins
const int fp2800a_pulse_length  = 150; // microseconds

// setup the objects for the FP2800a rows and columns controller
// Parameter order:                 Enable, Data,  A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800a rowController(   A0,     2,     3,  4,  5,  6,  7,   fp2800a_pulse_length);
FlipTheDot_FP2800a columnController(A1,     8,     9,  10, 11, 12, 13,  fp2800a_pulse_length);


// helper variables
int rowOutputNo = 0;
int rowOutputNoMax = rowController.getOutputMax();

int columnOutputNo = 0;
int columnOutputNoMax = columnController.getOutputMax();

boolean dataStatus = false;

int loopMaxRowOutputNo;

// variables to define the range of row IC outputs for the RESET action (should be 1-14)
int resetLoopMinOutputNo = 1;
int resetLoopMaxOutputNo = rowOutputNoMax / 2;

// variables to define the range of row IC outputs for the SET action (should be 15-28)
int setLoopMinOutputNo = rowOutputNoMax / 2 + 1;
int setLoopMaxOutputNo = rowOutputNoMax;


void setup() {
  // initialize debug Serial connection if defined
  #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
  FlipTheDot_FP2800a_DEBUG_SERIAL.begin(115200);
  #endif

  delay(1000);
  rowController.setData( dataStatus );
  columnController.setData( !dataStatus );
}


void loop() {
  // iterate thru one half of the row controller outputs, depending on the value of dataStatus
  rowOutputNo = (dataStatus ? setLoopMinOutputNo : resetLoopMinOutputNo);
  loopMaxRowOutputNo = (dataStatus ? setLoopMaxOutputNo : resetLoopMaxOutputNo);

  for ( rowOutputNo; rowOutputNo <= loopMaxRowOutputNo; rowOutputNo++ )
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
