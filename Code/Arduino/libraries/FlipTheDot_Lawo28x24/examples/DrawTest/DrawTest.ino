#define FlipTheDot_FP2800a_DEBUG 1
#define FlipTheDot_FP2800aMulti_DEBUG 1
#define FlipTheDot_ColumnRowController_DEBUG 1


// classes to control three FP2800a chips on a flipdot matrix module
#include "FlipTheDot_FP2800a.h";
FlipTheDot_FP2800a colsCtrl( 12,  A0, 7,  8,  9,  10, 11);
#include "FlipTheDot_FP2800aMulti.h";
int rowsEnablePinList = {A1};
FlipTheDot_FP2800aMulti rowsCtrl( rowsEnablePinList, 1, A2, 2,  3,  4,  5,  6);



// class to control a Lawo 28x13 module
#define FlipTheDot_Lawo28x24_DEBUG 1
#include "FlipTheDot_Lawo28x24.h"
FlipTheDot_Lawo28x24 lawo(colsCtrl, rowsCtrl);


// class which provide methods to "draw" onto the flipdot matrix
#include "FlipTheDot_Screen.h"
FlipTheDot_Screen screen(lawo);


void setup() {
    Serial.begin(115200);
    Serial.println("setup");
}


void loop() {
    Serial.println("loop");
    
    // fill row by row from top to bottom
    rowsFirst(screen);
    delay(1000);

    // fill row by row from left to right
    colsFirst(screen);
    delay(1000);

    while(1){};
}



/**
 * fill col by col from left to right
 */
void rowsFirst(FlipTheDot_IScreen &screen)
{
    int max_row = screen.height();
    int max_col = screen.width();
  
    // show
    for ( int r = 1; r <= max_row; r++ )
    {
        for ( int c = 1; c <= max_col; c++ )
        {
            screen.drawDot(c, r, true);
        }
    }
    delay(500);

    // hide
    for ( int r = 1; r <= max_row; r++ )
    {
        for ( int c = 1; c <= max_col; c++ )
        {
            screen.drawDot(c, r, false);
        }
    }
    delay(500);
}


/**
 * fill col by col from left to right
 */
void colsFirst(FlipTheDot_IScreen &screen)
{
    int max_row = screen.height();
    int max_col = screen.width();
  
    // show
    for ( int c = 1; c <= max_col; c++ )
    {
        for ( int r = 1; r <= max_row; r++ )
        {
            screen.drawDot(c, r, true);
        }
    }
    delay(500);

    // hide
    for ( int c = 1; c <= max_col; c++ )
    {
        for ( int r = 1; r <= max_row; r++ )
        {
            screen.drawDot(c, r, false);
        }
    }
    delay(500);
}
