#ifndef __setup_h__
#define __setup_h__



#if DEBUG
  #define SerialDebug Serial
  #define SerialDisplay Serial
#else
  #include <SoftwareSerial.h>
  extern SoftwareSerial SerialDebug;
  #define SerialDisplay Serial
#endif

#if DEBUG
  #define PRINT(s, x) { SerialDebug.print(F(s)); SerialDebug.print(x); }
  #define PRINTS(x) SerialDebug.print(x)
  #define PRINTSLN(x) SerialDebug.println(x)
  #define PRINTD(x) SerialDebug.println(x, DEC)
#else
  #define PRINT(s, x)
  #define PRINTS(x)
  #define PRINTSLN(x)
  #define PRINTD(x)
#endif


#endif
