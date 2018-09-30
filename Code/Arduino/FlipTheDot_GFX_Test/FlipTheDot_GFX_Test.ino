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

#include <Adafruit_GFX.h>
#include <gfxfont.h>

class FlipTheDot_GFX : public Adafruit_GFX {
  public:
    FlipTheDot_GFX(FlipTheDot_ColumnRowController &controller, int16_t cols, int16_t height);
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
  protected:
    FlipTheDot_ColumnRowController _controller;
};

FlipTheDot_GFX::FlipTheDot_GFX(FlipTheDot_ColumnRowController &controller, int16_t width, int16_t height) : Adafruit_GFX(width, height)
{
  _controller = controller;
}

void FlipTheDot_GFX::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  y = (_height-1) - y;
  
  y++;
  x++;

  y = color > 0 ? ((y + 14) + (y > 7 ? -7 : 7)) : y;
  _controller.flip(x, y, color > 0);
}

// defining the pulse length for the FP2800a enable pins
//const int fp2800a_pulse_length  = 100; // microseconds
const int fp2800a_pulse_length  = 150; // microseconds

// setup the objects for the FP2800a rows and columns controller
// Parameter order:                 Enable, Data,  A0, A1, A2, B0, B1,  Pulse length
//FlipTheDot_FP2800a rowController(   A0,     2,     3,  4,  5,  6,  7,   fp2800a_pulse_length);
//FlipTheDot_FP2800a columnController(A1,     8,     9,  10, 11, 12, 13,  fp2800a_pulse_length);
FlipTheDot_FP2800a columnController( 10,  A1,   A0,   12,   11,   A3,   A2);
FlipTheDot_FP2800a rowController( 2,   3,    4,    5,    6,    7,    8);


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

FlipTheDot_GFX gfx(controller, 28, 13);

void setup() {
  // initialize debug Serial connection if defined
  #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
  FlipTheDot_FP2800a_DEBUG_SERIAL.begin(9600);
  #endif
  #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
  FlipTheDot_ColumnRowController_DEBUG_SERIAL.begin(9600);
  #endif

  delay(1000);

  gfx.fillScreen(false);
}


void loop() {

  char *str = "Hello World!";
  int x = 0;
  int y = 0;

  gfx.setCursor(x, y);
  gfx.setTextColor(true, false);
  gfx.setTextSize(1);
  gfx.setTextWrap(false);

  y = 3;
  for ( x = gfx.width()-1; x >= -69; x-- )
  {
    gfx.setCursor(x, y);
    gfx.print(str);
    delay(100);
  }
  for ( x; x <= gfx.width(); x++ )
  {
    gfx.drawLine(x-1, y, x-1, y+7, false);
    gfx.setCursor(x, y);
    gfx.print(str);
    delay(100);
  }

  x = 0;
  for ( y = -7; y <= gfx.height(); y++ )
  {
    gfx.drawLine(x, y-1, gfx.width()-1, y-1, false);
    gfx.setCursor(x, y);
    gfx.print(str);
    delay(100);
  }
  for ( y; y >= -8; y-- )
  {
    gfx.drawLine(x, y+8, gfx.width()-1, y+8, false);
    gfx.setCursor(x, y);
    gfx.print(str);
    delay(100);
  }


  while(1){ delay(1000); }
  
  // iterate thru all rows
  for ( rowOutputNo = 0; rowOutputNo < gfx.height(); rowOutputNo++ )
  {
    // iterate thru all columns in the current row
    for ( columnOutputNo = 0; columnOutputNo < gfx.width(); columnOutputNo++ )
    {
      gfx.drawPixel(columnOutputNo, rowOutputNo, dataStatus);

      // remove/decrease this delay to speedup the process
      delay(10);
    }
  }

  // invert data flag
  dataStatus = !dataStatus;
}
