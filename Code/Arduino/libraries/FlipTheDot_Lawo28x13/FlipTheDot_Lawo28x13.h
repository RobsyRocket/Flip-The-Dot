/*
 * FlipTheDot_Lawo28x13 class -- Control of matrix with 28 columns and 13 rows from Lawo-Luminator-Europa Mark IV Industries GmbH
 *
 * @author Robert Römer <robert.roemer@live.de>
 *
 */



#ifndef FlipTheDot_Lawo28x13_h
#define FlipTheDot_Lawo28x13_h

#ifndef FlipTheDot_Lawo28x13_DEBUG
#define FlipTheDot_Lawo28x13_DEBUG 0
#endif

#include "FlipTheDot_FP2800a.h"
#include "FlipTheDot_ColumnRowController.h"
#include "FlipTheDot_Matrix.h"



/**
 * Special kind of FlipTheDot_Matrix 
 * The FlipTheDot_Lawo28x13 is a 28 cols by 13 row matrix which utilize two FP2800a pins for each row to provide the "show" and "hide" function.
 * The 13 available rows will be converted internaly to 26 for proper functionality.
 */
class FlipTheDot_Lawo28x13 : public FlipTheDot_Matrix
{
    public:
        FlipTheDot_Lawo28x13(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int pulseLengthMicros);
        virtual boolean flip(unsigned int col, unsigned int row, boolean show);
    
    protected:
        FlipTheDot_Lawo28x13(){};
};



FlipTheDot_Lawo28x13::FlipTheDot_Lawo28x13(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int pulseLengthMicros = 100)
{
    _cols = 28;
    _rows = 13;
    _ctrl = FlipTheDot_ColumnRowController(col_ctrl, row_ctrl, _cols, _rows*2, pulseLengthMicros);
}


boolean FlipTheDot_Lawo28x13::flip(unsigned int col, unsigned int row, boolean show)
{
    #if FlipTheDot_Lawo28x13_DEBUG == 1
    Serial.print("FlipTheDot_Lawo28x13::flip(");Serial.print(col);Serial.print(", ");Serial.print(row);Serial.print(", ");Serial.print(show);Serial.println(")");
    #endif
    if ( row > _rows || col > _cols )
    {
        return false;
    }
    else
    {
        // flip/mirror column no to set the orientation to top left
        return _ctrl.flip( (_cols+1)-col, (show ? row*2-1 : row*2), show);
    }
}



#endif  // FlipTheDot_Lawo28x13_h
