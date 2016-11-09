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


  A almost identical code can be found in the example Controlling_Rows_and_Columns/DefaultDefault of the 
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
#ifndef FlipTheDot_ColumnRowController_DEBUG_SERIAL
#define FlipTheDot_ColumnRowController_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800a.h"
#include "FlipTheDot_ColumnRowController.h"


// defining the pulse length for the FP2800a enable pins
const int fp2800a_pulse_length  = 100; // microseconds

// setup the objects for the FP2800a rows and columns controller
// Parameter order:                 Enable, Data,  A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800a rowController(    2,      3,     4,  5,  6,  7,  8,  fp2800a_pulse_length);
FlipTheDot_FP2800a columnController(10,     A1,    A0, 12, 11, A3, A2,  fp2800a_pulse_length);


// wrap independent column and row controllers in one object for easier usage
FlipTheDot_ColumnRowController controller(columnController, rowController, fp2800a_pulse_length);


// helper variables
int rowOutputNo = 0;
int rowOutputNoMax = rowController.getOutputMax();

int columnOutputNo = 0;
int columnOutputNoMax = columnController.getOutputMax();

boolean dataStatus = true;

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
  FlipTheDot_FP2800a_DEBUG_SERIAL.begin(9600);
  #endif
  #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
  FlipTheDot_ColumnRowController_DEBUG_SERIAL.begin(9600);
  #endif

  delay(1000);
}


void loop() {
  // iterate thru one half of the row controller outputs, depending on the value of dataStatus
  rowOutputNo = (dataStatus ? setLoopMinOutputNo : resetLoopMinOutputNo);
  loopMaxRowOutputNo = (dataStatus ? setLoopMaxOutputNo : resetLoopMaxOutputNo);

  for ( rowOutputNo; rowOutputNo <= loopMaxRowOutputNo; rowOutputNo++ )
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
