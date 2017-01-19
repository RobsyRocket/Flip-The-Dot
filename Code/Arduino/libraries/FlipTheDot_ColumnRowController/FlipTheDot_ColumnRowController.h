/*
 * FlipTheDot_ColumnRowController Class  -- Control the outputs of two FP2800a chips
 *
 * @author Robert Römer <robert.roemer@live.de>
 *
 */



#ifndef FlipTheDot_ColumnRowController_h
#define FlipTheDot_ColumnRowController_h


#include "FlipTheDot_FP2800a.h"


class FlipTheDot_ColumnRowController
{
    public:
        FlipTheDot_ColumnRowController(){};
        FlipTheDot_ColumnRowController(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int cols, unsigned int rows, unsigned int pulseLengthMicros);
        FlipTheDot_ColumnRowController(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int pulseLengthMicros);
        void setPulseLength(unsigned int pulseLengthMicros);
        unsigned int getPulseLength();
        boolean show(unsigned int col, unsigned int row);
        boolean hide(unsigned int col, unsigned int row);
        boolean flip(unsigned int col, unsigned int row, boolean show);
    protected:
        FlipTheDot_FP2800a *_colCtrl;
        FlipTheDot_FP2800a *_rowCtrl;

        unsigned int _cols = 0;
        unsigned int _rows = 0;
        unsigned int _pulseLengthMicros = 0;
};



FlipTheDot_ColumnRowController::FlipTheDot_ColumnRowController(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int pulseLengthMicros = 100)
{
    _rowCtrl = &row_ctrl;
    _colCtrl = &col_ctrl;

    _cols = _colCtrl->getOutputMax();
    _rows = _rowCtrl->getOutputMax();

    _pulseLengthMicros = pulseLengthMicros;
}

FlipTheDot_ColumnRowController::FlipTheDot_ColumnRowController(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int cols, unsigned int rows, unsigned int pulseLengthMicros = 100)
{
    _rowCtrl = &row_ctrl;
    _colCtrl = &col_ctrl;

    _cols = cols;
    _rows = rows;

    _pulseLengthMicros = pulseLengthMicros;
}

/**
 * define how long the pulse method should enable the controllers
 */
void FlipTheDot_ColumnRowController::setPulseLength(unsigned int pulseLengthMicros)
{
    _pulseLengthMicros = pulseLengthMicros;
}

/**
 * get pulse length
 */
unsigned int FlipTheDot_ColumnRowController::getPulseLength()
{
    return _pulseLengthMicros;
}

boolean FlipTheDot_ColumnRowController::show(unsigned int col, unsigned int row)
{
    #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.println( F("FlipTheDot_ColumnRowController show") );
    #endif
    return flip(col, row, true);
}

boolean FlipTheDot_ColumnRowController::hide(unsigned int col, unsigned int row)
{
    #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.println( F("FlipTheDot_ColumnRowController hide") );
    #endif
    return flip(col, row, false);
}

boolean FlipTheDot_ColumnRowController::flip(unsigned int col, unsigned int row, boolean show)
{
    boolean row_data = show == true;
    boolean col_data = !row_data;

    // invalid range
    if ( row < 1 || row > _rows || col < 1 || col > _cols )
    {
      #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
      FlipTheDot_ColumnRowController_DEBUG_SERIAL.println( F("FlipTheDot_ColumnRowController selected row or column number out of range") );
      #endif
      return false;
    }

    #ifdef FlipTheDot_ColumnRowController_DEBUG_SERIAL
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print( F("FlipTheDot_ColumnRowController flip ") );
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print(show ? F("(show)") : F("(hide)"));
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print( F(" col ") );
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print(col);
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print( F(" row ") );
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print(row);
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print( F(" with ") );
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.print(_pulseLengthMicros);
    FlipTheDot_ColumnRowController_DEBUG_SERIAL.println( F(" microseconds pulse. ") );
    #endif

    if ( _colCtrl->setOutput(col) && _rowCtrl->setOutput(row) )
    {
        _rowCtrl->setData(row_data);
        _colCtrl->setData(col_data);

        _colCtrl->enable();
        _rowCtrl->enable();
        delayMicroseconds(_pulseLengthMicros);
        _colCtrl->disable();
        _rowCtrl->disable();

        return true;
    }
    return false;
}



#endif // FlipTheDot_ColumnRowController_h