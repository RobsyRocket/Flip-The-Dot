/*
 * FlipTheDot_Lawo28x24 class -- Control of matrix with 28 columns and 24 rows from Lawo-Luminator-Europa Mark IV Industries GmbH
 *
 * @author Robert Römer <robert.roemer@live.de>
 *
 */



#ifndef FlipTheDot_Lawo28x24_h
#define FlipTheDot_Lawo28x24_h

#ifndef FlipTheDot_Lawo28x24_DEBUG
#define FlipTheDot_Lawo28x24_DEBUG 0
#endif

#include "FlipTheDot_FP2800a.h"
#include "FlipTheDot_FP2800aMulti.h"
#include "FlipTheDot_ColumnRowController.h"
#include "FlipTheDot_Matrix.h"



/**
 * Special kind of FlipTheDot_Matrix 
 * The FlipTheDot_Lawo28x24 is a 28 columns by 13 row matrix which utilize two FP2800a chips for each row to provide the "show" and "hide" function.
 * The 13 available rows will be converted internaly to 26 for proper functionality. One half group for the show/set function the otherone for hide/reset.
 */
class FlipTheDot_Lawo28x24 : public FlipTheDot_Matrix
{
    public:
        FlipTheDot_Lawo28x24(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800aMulti &row_ctrl, unsigned int pulseLengthMicros);
        virtual boolean flip(unsigned int col, unsigned int row, boolean show);

    protected:
        FlipTheDot_Lawo28x24(){};
};


FlipTheDot_Lawo28x24::FlipTheDot_Lawo28x24(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800aMulti &row_ctrl, unsigned int pulseLengthMicros = 100)
{
    _cols = 28;
    _rows = 24;
    _ctrl = FlipTheDot_ColumnRowController(col_ctrl, row_ctrl, _cols, _rows, pulseLengthMicros);
}


boolean FlipTheDot_Lawo28x24::flip(unsigned int col, unsigned int row, boolean show)
{
    #if FlipTheDot_Lawo28x24_DEBUG_SERIAL
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.print( F("FlipTheDot_Lawo28x24::flip(") ;
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.print(col);
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.print( F(", ") );
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.print(row);
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.print( F(", ") ;
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.print(show);
    FlipTheDot_Lawo28x24_DEBUG_SERIAL.println( F(")") ;
    #endif

    if ( (row > 0 && row <= MAX_ROWS) && (col > 0 && col <= MAX_COLS))
    {
        row = (!is_high || row < 1) ? row : (row + (row > 7 ? 7 : 21));
        //return controller.flip(col, row_remapped, is_high);
        return _ctrl.flip( (_cols+1)-col, row, show);
        return _ctrl.flip( (_cols+1)-col, (show ? row*2-1 : row*2), show);
    }

};



#endif  // FlipTheDot_Lawo28x24_h
