Abstractation base for different flip dot matrix displayes with uniform class definition. It utilize other parts of the FlipTheDot project and should provide the posibility to capsulate hardware specific details.
This library includes the interface FlipTheDot_IMatrix and bootstrapped class FlipTheDot_Matrix for custom implementation.

Debugging via serial communication can be activated at compile time by adding a define like the following one:
```
#define FlipTheDot_Matrix_DEBUG_SERIAL Serial
```
