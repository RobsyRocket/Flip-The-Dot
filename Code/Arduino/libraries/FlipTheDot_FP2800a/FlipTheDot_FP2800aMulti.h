/*
 * FlipTheDot_FP2800aMulti Class
 * 
 * Works like the normal FP2800a class but utilize multiple ICs and maps the selected output to the correct IC.
 * The several control pins (A0-B1 and Data) of all ICs are wired to the same microcontroller pins, except the enable pins which are separated and get individually controlled.
 *
 * @author Robert Römer <robert.roemer@live.de>
 */



#ifndef FlipTheDot_FP2800aMulti_h
#define FlipTheDot_FP2800aMulti_h

#include "Arduino.h"
#include "FlipTheDot_FP2800a.h"



class FlipTheDot_FP2800aMulti : public FlipTheDot_FP2800a
{
    public:
        // nearly identical parameters like FP2800a but with enable pin list and its length informations
        FlipTheDot_FP2800aMulti(unsigned int pinEnableList[], unsigned int pinEnableListLength, unsigned int pinData, unsigned int pinA0, unsigned int pinA1, unsigned int pinA2, unsigned int pinB0, unsigned int pinB1, unsigned int pulseLengthMillis);
        bool setOutput(unsigned int no);
        unsigned int getOutput();
        unsigned int getOutputMax();
        void FlipTheDot_FP2800aMulti::_initPins();
    
    protected:
        byte _selectedEnableNo = 0;
        unsigned int *_pinEnableList;
        unsigned int _pinEnableListLength = 0;
        bool _hasDuplicatePins();
};


FlipTheDot_FP2800aMulti::FlipTheDot_FP2800aMulti(unsigned int pinEnableList[], unsigned int pinEnableListLength, unsigned int pinData, unsigned int pinA0, unsigned int pinA1, unsigned int pinA2, unsigned int pinB0, unsigned int pinB1, unsigned int pulseLengthMillis = 100) : FlipTheDot_FP2800a(pinEnableList[0], pinData, pinA0, pinA1, pinA2, pinB0, pinB1, pulseLengthMillis)
{
    // the value for key 0 of pinEnableList gets immediatly forwarded to the parent constructor

    // storing the provided list and length of enable pins
    _pinEnableList = pinEnableList;
    _pinEnableListLength = pinEnableListLength;
    
    // following instruction are already called in the parent constructor but need to be called again to count in the _pinEnableList values
    {
        // check for duplicates
        if ( _hasDuplicatePins() )
        {
          // some of the pins are equal => not allowed
          #ifdef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
          //FlipTheDot_FP2800aMulti_DEBUG_SERIAL.println( F("Pin configuration for FlipTheDot_FP2800aMulti is not valid: duplicate pin detected") );
          #endif
          while(1);
        }

        // call initPins again, like in the parent constructor, to check the whole list of enable pins
        _initPins();
    }
}


void FlipTheDot_FP2800aMulti::_initPins()
{
    // call parent _initPins method 
    FlipTheDot_FP2800a::_initPins();
    
    // loop thru all enable pins, except the first one which was initialized in the parent _initPins call
    for ( byte i=0; i < _pinEnableListLength; i++)
    {
        pinMode(_pinEnableList[i], OUTPUT);
        digitalWrite(_pinEnableList[i], LOW);
    }
}

/**
 * Check if one or more pins are identical and referencing the same hardware IO.
 * The list of all enable pins will be compared to the other functional pin defintions.
 */
bool FlipTheDot_FP2800aMulti::_hasDuplicatePins()
{
    // loop thru the list of all enable pins
    for ( byte i=0; i < _pinEnableListLength; i++)
    {
        // temporary change selected enable pin no
        _pinEnable = _pinEnableList[i];

        // run the regular duplicate check
        if ( FlipTheDot_FP2800a::_hasDuplicatePins() )
        {
            // set the enable pin back to the one which was set before this method call
            _pinEnable = _pinEnableList[_selectedEnableNo];

            // stop because duplicate detected
            return true;
        }
    }
    // set the enable pin back to the one which was set before this method call
    _pinEnable = _pinEnableList[_selectedEnableNo];
        
    // no duplicate detected
    return false;
}


/**
 * Map the output no to the corresponding enable pin and its outputs.
 */
bool FlipTheDot_FP2800aMulti::setOutput(unsigned int no)
{
    // calculate which enable pin (chip / display) should be selected
    byte enable_no = no / _maxOutputsOnChip;

    // recalculation of the output in relation to the selected enable pin
    no = no % _maxOutputsOnChip;
    
    // special handling if previous calculation returning zero
    if ( no == 0 )
    {
        enable_no -= enable_no > 0 ? 1 : 0;
        no = _maxOutputsOnChip;
    }
    
    // check if the enable_no is out of range
    if ( enable_no >= _pinEnableListLength )
    {
        #ifdef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
        FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F("FlipTheDot_FP2800aMulti selected enable pin ") );
        FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(no);
        FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F(" is out of range 0 to ") );
        FlipTheDot_FP2800aMulti_DEBUG_SERIAL.println(_pinEnableListLength-1);
        #endif
        return false;
    }
    
    // check if the desired output is not already selected (ignoring the previously selected enable pin)
    if ( _selectedOutput != no )
    {
        // try to set the recalculated output no like defined in the parent implementation
        if ( !FlipTheDot_FP2800a::setOutput(no) )
        {
            // output could not be set, the enable pin was maybe active
            return false;
        }
    }

    // change enable pin
    _selectedEnableNo = enable_no;
    _pinEnable = _pinEnableList[_selectedEnableNo];
    
    #ifdef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F("FlipTheDot_FP2800aMulti output ") );
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(enable_no*_maxOutputsOnChip+no);
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F(" selected (mapped to ") );
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(no);
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F(" and enable pin ") );
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(enable_no+1);
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.println( F(")") );
    #endif
    return true;
}


/**
 * get selected output
 */
unsigned int FlipTheDot_FP2800aMulti::getOutput()
{
    return _selectedOutput + (_selectedEnableNo * _maxOutputsOnChip);
}


/**
 * get maximum output number
 */
unsigned int FlipTheDot_FP2800aMulti::getOutputMax()
{
    // Dont active if this method gets called before FlipTheDot_FP2800aMulti_DEBUG_SERIAL.begin(...)
    /* 
    #ifdef FlipTheDot_FP2800aMulti_DEBUG_SERIAL
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F("FlipTheDot_FP2800aMulti max outputs: ") );
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(_maxOutputsOnChip * _pinEnableListLength);
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F(" (") );
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(_maxOutputsOnChip);
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print( F(" * ") );
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.print(_pinEnableListLength);
    FlipTheDot_FP2800aMulti_DEBUG_SERIAL.println( F(")") );
    #endif
    */
    return _maxOutputsOnChip * _pinEnableListLength;
}




#endif // FlipTheDot_FP2800aMulti_h