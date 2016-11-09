/*
  Fixed-Multi Controller
  Iterate thru the output numbers of multiple controllers in a row and multi column group configuration.
  
  The script is identical to the example "Fixed-Default" but utilize more FP2800a to control multiple 
  column groups.

  This example code is in the public domain.

  modified 09 November 2016
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
#ifndef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
#define FlipTheDot_FP2800aMulti_DEBUG_SERIAL Serial
#endif
#ifndef FlipTheDot_ColumnRowController_DEBUG_SERIAL
#define FlipTheDot_ColumnRowController_DEBUG_SERIAL Serial
#endif
*/

// include the library
#include "FlipTheDot_FP2800aFixed.h"
#include "FlipTheDot_FP2800aMulti.h"
#include "FlipTheDot_ColumnRowController.h"


// defining the pulse length for the FP2800a enable pins
const int fp2800a_pulse_length  = 100; // microseconds

// prepare the list of columns enable pins for the available FP2800a ICs
const int columnEnableList[] = {
  A1, // first columns group
  A2, // second
  A3, // ...
  // ...
};
const int columnEnableListLength = sizeof(columnEnableList)/sizeof(int);

// setup the objects for the fixed FP2800a rows controller and FP2800aMulti columns controller
// Parameter order:                      Enable Reset,                             Enable Set,  A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800aFixed rowController(   A0,                                       2,           3,  4,  5,  6,  7,   fp2800a_pulse_length);
// Parameter order:                      Enable List,      Enable List length      Data,        A0, A1, A2, B0, B1,  Pulse length
FlipTheDot_FP2800aMulti columnController(columnEnableList, columnEnableListLength, 8,           9,  10, 11, 12, 13,  fp2800a_pulse_length);


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
  #ifdef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
  FlipTheDot_FP2800aMulti_DEBUG_SERIAL.begin(9600);
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
