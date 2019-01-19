#ifndef __setup_h__
#define __setup_h__


#define SerialDisplay Serial


#if DEBUG == 1
  #warning Setting up Hardware serial for debugging on the same connection as the display serial
  #define SerialDebug Serial
#endif
#if DEBUG == 2
  #warning Setting up SoftwareSerial debugging
  #include "SoftwareSerial.h"
  SoftwareSerial SerialDebug(14, 12, false, 256);
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
