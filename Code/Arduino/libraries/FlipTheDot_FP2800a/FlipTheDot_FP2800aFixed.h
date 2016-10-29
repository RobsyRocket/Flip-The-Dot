/*
 * FlipTheDot_FP2800aFixed Class
 * 
 * Works like the normal FP2800a class but instead of toggling the data pin, 
 * this implementation switches the enable pin number of two ICs with fixed data pins.
 * Both ICs use the same and connected output selection pins (A0, A1, A2, B0 and B1) but have fixed
 * data pins (one Data is HIGH and the other is LOW) and individual Enable pins.
 *
 * @author Robert Römer <robert.roemer@live.de>
 */


#ifndef FlipTheDot_FP2800aFixed_h
#define FlipTheDot_FP2800aFixed_h

#include "Arduino.h"
#include "FlipTheDot_FP2800a.h"



class FlipTheDot_FP2800aFixed : public FlipTheDot_FP2800a
{
    public:
        // identical to FP2800a but with renamed parameters and different interal handling
        FlipTheDot_FP2800aFixed(unsigned int pinEnableReset, unsigned int pinEnableSet, unsigned int pinA0, unsigned int pinA1, unsigned int pinA2, unsigned int pinB0, unsigned int pinB1, unsigned int pulseLengthMillis);
        bool setData(bool is_high);

    protected:
        // additional variables to hold the enable pin numbers for the both functions
        // the _pinEnable variable of the parent class get changed in the method setData(...) when needed
        unsigned int _pinEnableReset;
        unsigned int _pinEnableSet;
};


FlipTheDot_FP2800aFixed::FlipTheDot_FP2800aFixed(unsigned int pinEnableReset, unsigned int pinEnableSet, unsigned int pinA0, unsigned int pinA1, unsigned int pinA2, unsigned int pinB0, unsigned int pinB1, unsigned int pulseLengthMillis = 100) : FlipTheDot_FP2800a(pinEnableReset, pinEnableSet, pinA0, pinA1, pinA2, pinB0, pinB1, pulseLengthMillis)
{
    // The first parameter is defined as reset, because this matches the default behaviour of the parent class where the _pinData
    // is default LOW and a call to enable() or pulse() results in a reset action - without previous calling setData(...).
    
    // Only special storing of the first two parameter values required.
    // The parent class constructor checks all pins for duplicates, sets _pinEnable = pinEnableReset and 
    // initialize all pins (mode and default state).
    _pinEnableReset = pinEnableReset;
    _pinEnableSet   = pinEnableSet;
}


/**
 * define if the output should source or sink current
 * instead of toggling the data pin state, this implementation switches the enable pin number
 */
bool FlipTheDot_FP2800aFixed::setData(bool is_high)
{
    if ( isEnabled() == true )
    {
        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.println( F("FlipTheDot_FP2800a data cannot be changed when IC is already enabled") );
        #endif
        return false;
    }

    _pinEnable = is_high == true ? _pinEnableSet : _pinEnableReset;
    
    #ifdef FlipTheDot_FP2800aFixed_DEBUG_SERIAL
    FlipTheDot_FP2800aFixed_DEBUG_SERIAL.print( F("FlipTheDot_FP2800aFixed data updated by switching enable pin numbers (") );
    FlipTheDot_FP2800aFixed_DEBUG_SERIAL.print(_pinEnable == _pinEnableSet ? F("HIGH") : F("LOW"));
    FlipTheDot_FP2800aFixed_DEBUG_SERIAL.println( F(")") );
    #endif

    return true;
}



#endif // FlipTheDot_FP2800aFixed_h