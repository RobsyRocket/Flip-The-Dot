/*
 * FlipTheDot_FP2800a Class  -- Control the 28 ports on a FP2800a chip
 *
 * @author Robert Römer <robert.roemer@live.de>
 */



#ifndef FlipTheDot_FP2800a_h
#define FlipTheDot_FP2800a_h

#include "Arduino.h"



class FlipTheDot_FP2800a
{
    public:
        FlipTheDot_FP2800a(){};
        FlipTheDot_FP2800a(unsigned int pinData, unsigned int pinEnable, unsigned int pinA0, unsigned int pinA1, unsigned int pinA2, unsigned int pinB0, unsigned int pinB1, unsigned int pulseLengthMicros);
        ~FlipTheDot_FP2800a();
        virtual void pulse();
        virtual bool setOutput(unsigned int no);
        virtual unsigned int getOutput();
        virtual bool setData(bool is_high);
        virtual void setPulseLength(unsigned int pulseLengthMicros);
        virtual unsigned int getPulseLength();
        virtual void enable();
        virtual void disable();
        virtual bool isEnabled();
    
    protected:
        void initPins();

        virtual bool _hasDuplicatePins();
        unsigned int _pulseLengthMicros;

        unsigned int _pinData;
        unsigned int _pinEnable;

        unsigned int _pinA0;
        unsigned int _pinA1;
        unsigned int _pinA2;

        unsigned int _pinB0;
        unsigned int _pinB1;

        unsigned int _selectedOutput;

        bool _isEnabled = false;
};



FlipTheDot_FP2800a::FlipTheDot_FP2800a(unsigned int pinData, unsigned int pinEnable, unsigned int pinA0, unsigned int pinA1, unsigned int pinA2, unsigned int pinB0, unsigned int pinB1, unsigned int pulseLengthMicros = 100)
{
    _pulseLengthMicros = pulseLengthMicros;

    _pinData = pinData;
    _pinEnable = pinEnable;

    _pinA0 = pinA0;
    _pinA1 = pinA1;
    _pinA2 = pinA2;

    _pinB0 = pinB0;
    _pinB1 = pinB1;

    // check for duplicates
    if ( _hasDuplicatePins() )
    {
      // some of the pins are equal => not allowed
      #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
      //FlipTheDot_FP2800a_DEBUG_SERIAL.println("Pin configuration for FlipTheDot_FP2800a is not valid: duplicate pin detected");
      #endif
      while(1);
    }

    _selectedOutput = 0;

    initPins();
}


FlipTheDot_FP2800a::~FlipTheDot_FP2800a()
{
    // set all pins low to avoid burnout of the chip
    digitalWrite(_pinData, LOW);
    digitalWrite(_pinEnable, LOW);

    digitalWrite(_pinA0, LOW);
    digitalWrite(_pinA1, LOW);
    digitalWrite(_pinA2, LOW);

    digitalWrite(_pinB0, LOW);
    digitalWrite(_pinEnable, LOW);
}


/**
 * check if one or more pins are identical and referencing the same hardware IO
 */
bool FlipTheDot_FP2800a::_hasDuplicatePins()
{
    if (
        _pinData == _pinEnable || _pinData == _pinA0 || _pinData == _pinA1 || _pinData == _pinA2 || _pinData == _pinB0 || _pinData == _pinB1 ||
        _pinEnable == _pinA0 || _pinEnable == _pinA1 || _pinEnable == _pinA2 || _pinEnable == _pinB0 || _pinEnable == _pinB1 ||
        _pinA0 == _pinA1 || _pinA0 == _pinA2 || _pinA0 == _pinB0 || _pinA0 == _pinB1 ||
        _pinA1 == _pinA2 || _pinA1 == _pinB0 || _pinA1 == _pinB1 ||
        _pinA2 == _pinB0 || _pinA2 == _pinB1 ||
        _pinB0 == _pinB1
    )
    {
      return true;
    }
    return false;
}


void FlipTheDot_FP2800a::initPins()
{
    pinMode(_pinData, OUTPUT);
    digitalWrite(_pinData, LOW);

    pinMode(_pinEnable, OUTPUT);
    digitalWrite(_pinEnable, LOW);

    pinMode(_pinA0, OUTPUT);
    digitalWrite(_pinA0, LOW);
    pinMode(_pinA1, OUTPUT);
    digitalWrite(_pinA1, LOW);
    pinMode(_pinA2, OUTPUT);
    digitalWrite(_pinA2, LOW);

    pinMode(_pinB0, OUTPUT);
    digitalWrite(_pinB0, LOW);
    pinMode(_pinB1, OUTPUT);
    digitalWrite(_pinEnable, LOW);

    #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
    FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a pins initialized");
    #endif
}


/**
 * define if the output should source or sink current
 * can only be changed when enabled is low
 */
bool FlipTheDot_FP2800a::setData(bool is_high)
{
    if ( isEnabled() == true )
    {
        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a data cannot be changed when IC is already enabled");
        #endif
        return false;
    }


    #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
    Serial.println("FlipTheDot_FP2800a data updated");
    #endif

    digitalWrite(_pinData, is_high == true ? HIGH : LOW);
    
    return true;
}


/**
 * define how long the pulse method should enable the controller
 */
void FlipTheDot_FP2800a::setPulseLength(unsigned int pulseLengthMicros)
{
    _pulseLengthMicros = pulseLengthMicros;
}


/**
 * get pulse length
 */
unsigned int FlipTheDot_FP2800a::getPulseLength()
{
    return _pulseLengthMicros;
}


/**
 * switch pins (A0, A1, A2, B0, B1) to HIGH/LOW to define which pin should be enabled during F2800::pulse
 * can only be changed when enabled is low
 */
bool FlipTheDot_FP2800a::setOutput(unsigned int no)
{
    if ( isEnabled() == true )
    {
        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a output cannot be selected when IC is already enabled");
        #endif
        return false;
    }
    
    if ( _selectedOutput == no )
    {
        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.print("FlipTheDot_FP2800a Output already selected: ");
        FlipTheDot_FP2800a_DEBUG_SERIAL.println(no);
        #endif
        return true;
    }
    else if ( no < 1 || no > 28 )
    {
        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.print("FlipTheDot_FP2800a selected output ");
        FlipTheDot_FP2800a_DEBUG_SERIAL.print(no);
        FlipTheDot_FP2800a_DEBUG_SERIAL.println(" is out of range 1 to 28");
        #endif
        return false;
    }
    else
    {
        _selectedOutput = no;
        if ( no > 14 )
        {
            // 1????
            digitalWrite(_pinB1, HIGH);
            no = no-14;
        }
        else
        {
            // 0????
            digitalWrite(_pinB1, LOW);
        }

        if ( no > 7 )
        {
            // ?1???
            digitalWrite(_pinB0, HIGH);
            no = no-7;
        }
        else
        {
            // ?0???
            digitalWrite(_pinB0, LOW);
        }

        switch (no)
        {
            case 1: // ??001
                digitalWrite(_pinA2, LOW);
                digitalWrite(_pinA1, LOW);
                digitalWrite(_pinA0, HIGH);
            break;
            case 2: // ??010
                digitalWrite(_pinA2, LOW);
                digitalWrite(_pinA1, HIGH);
                digitalWrite(_pinA0, LOW);
            break;
            case 3: // ??011
                digitalWrite(_pinA2, LOW);
                digitalWrite(_pinA1, HIGH);
                digitalWrite(_pinA0, HIGH);
            break;
            case 4: // ??100
                digitalWrite(_pinA2, HIGH);
                digitalWrite(_pinA1, LOW);
                digitalWrite(_pinA0, LOW);
            break;
            case 5: // ??101
                digitalWrite(_pinA2, HIGH);
                digitalWrite(_pinA1, LOW);
                digitalWrite(_pinA0, HIGH);
            break;
            case 6: // ??110
                digitalWrite(_pinA2, HIGH);
                digitalWrite(_pinA1, HIGH);
                digitalWrite(_pinA0, LOW);
            break;
            case 7: // ??111
                digitalWrite(_pinA2, HIGH);
                digitalWrite(_pinA1, HIGH);
                digitalWrite(_pinA0, HIGH);
            break;
        }

        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.print("FlipTheDot_FP2800a output ");
        FlipTheDot_FP2800a_DEBUG_SERIAL.print(_selectedOutput);
        FlipTheDot_FP2800a_DEBUG_SERIAL.println(" selected");
        #endif

        return true;
    }
}


/**
 * get selected output
 */
unsigned int FlipTheDot_FP2800a::getOutput()
{
    return _selectedOutput;
}


/**
 * enable the selected port to source or sink power
 * use with caution because the controller keeps enabled until FlipTheDot_FP2800a::disabled get called
 */
void FlipTheDot_FP2800a::enable()
{
    #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
    FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a enabling");
    #endif
    _isEnabled = true;
    digitalWrite(_pinEnable, HIGH);
}


/**
 * disable/close the selected port
 */
void FlipTheDot_FP2800a::disable()
{
    digitalWrite(_pinEnable, LOW);
    _isEnabled = false;
    #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
    FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a disabled");
    #endif
}


/**
 * get information if the enable pin is currently HIGH
 */
bool FlipTheDot_FP2800a::isEnabled()
{
    #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
    FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a get enable state");
    #endif
    return _isEnabled;
}


/**
 * enable the selected port for a given time (defined by the pulse length)
 */
void FlipTheDot_FP2800a::pulse()
{
    if ( _pulseLengthMicros > 0 )
    {
        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.print("FlipTheDot_FP2800a start pulse with length ");
        FlipTheDot_FP2800a_DEBUG_SERIAL.print(_pulseLengthMicros);
        FlipTheDot_FP2800a_DEBUG_SERIAL.println(" microseconds");
        #endif

        enable();
        delayMicroseconds(_pulseLengthMicros);
        disable();

        #ifdef FlipTheDot_FP2800a_DEBUG_SERIAL
        FlipTheDot_FP2800a_DEBUG_SERIAL.println("FlipTheDot_FP2800a finished pulse");
        #endif
    }
}



#endif // FlipTheDot_FP2800a_h