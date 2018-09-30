#ifndef FlipTheDot_Matrix_h
#define FlipTheDot_Matrix_h

#ifndef FlipTheDot_Matrix_DEBUG
#define FlipTheDot_Matrix_DEBUG 0
#endif


#include "FlipTheDot_FP2800a.h"
#include "FlipTheDot_ColumnRowController.h"


class FlipTheDot_IMatrix
{
  public:
    virtual unsigned int height() = 0;
    virtual unsigned int width() = 0;
    virtual boolean flip(unsigned int col, unsigned int row, boolean show) = 0;
    virtual boolean show(unsigned int col, unsigned int row) = 0;
    virtual boolean hide(unsigned int col, unsigned int row) = 0;
};



class FlipTheDot_Matrix : public FlipTheDot_IMatrix
{
    public:
        FlipTheDot_Matrix(){};
        FlipTheDot_Matrix(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int cols, unsigned int rows, float pulseLengthMicros);
        unsigned int width();
        unsigned int height();
        boolean flip(unsigned int col, unsigned int row, boolean show);
        boolean show(unsigned int col, unsigned int row);
        boolean hide(unsigned int col, unsigned int row);

    protected:
        FlipTheDot_ColumnRowController _ctrl;

        unsigned int _rows;
        unsigned int _cols;
};



FlipTheDot_Matrix::FlipTheDot_Matrix(FlipTheDot_FP2800a &col_ctrl, FlipTheDot_FP2800a &row_ctrl, unsigned int cols, unsigned int rows, float pulseLengthMicros = 100)
{
    _cols = cols;
    _rows = rows;
    _ctrl = FlipTheDot_ColumnRowController(col_ctrl, row_ctrl, _cols, _rows, pulseLengthMicros);
};


unsigned int FlipTheDot_Matrix::width()
{
    return _cols;
};


unsigned int FlipTheDot_Matrix::height()
{
    return _rows;
};


boolean FlipTheDot_Matrix::flip(unsigned int col, unsigned int row, boolean show)
{
    #if FlipTheDot_Matrix_DEBUG == 1
    Serial.print("FlipTheDot_Matrix::flip(");Serial.print(col);Serial.print(", ");Serial.print(row);Serial.print(", ");Serial.print(show);Serial.println(")");
    #endif
    return _ctrl.flip(col, row, show);
};


boolean FlipTheDot_Matrix::show(unsigned int col, unsigned int row)
{
    return flip(col, row, true);
};


boolean FlipTheDot_Matrix::hide(unsigned int col, unsigned int row)
{
    return flip(col, row, false);
};



#endif //  FlipTheDot_Matrix_h