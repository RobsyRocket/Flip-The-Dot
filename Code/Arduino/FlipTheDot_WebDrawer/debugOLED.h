/**
The following code serves debuging purposes and enables the use of a Lolin(Wemos) OLED display without actually connecting a real flipdot display controller.

Just add the following statement to the main file before including this one.
#define DEBUG_OLED 1

 */
#if defined(DEBUG_OLED) && DEBUG_OLED

#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C Example

unsigned int OLED_GAP = 1;
unsigned int OLED_BORDER = 1;

unsigned int OLED_seg_width = 2;
unsigned int OLED_seg_height = 2;
unsigned int OLED_seg_width_with_border = OLED_seg_width+OLED_BORDER*2;
unsigned int OLED_seg_height_with_border = OLED_seg_height+OLED_BORDER*2;

#define OLED_X_LIMIT  (64 + OLED_GAP) / (OLED_seg_width_with_border+OLED_GAP)
#define OLED_Y_LIMIT (48 + OLED_GAP) / (OLED_seg_height_with_border+OLED_GAP)

unsigned int OLED_x_max = OLED_X_LIMIT;
unsigned int OLED_y_max = OLED_Y_LIMIT;


int OLED_x, OLED_y = 0;

void debugOLEDInitField(unsigned int size_x, unsigned int size_y) {
  OLED_x_max = (size_x < OLED_X_LIMIT) ? size_x : OLED_X_LIMIT;
  OLED_y_max = (size_y < OLED_Y_LIMIT) ? size_y : OLED_Y_LIMIT;

  oled.clear(ALL); // will clear out the OLED's graphic memory.
  oled.clear(PAGE); // will clear the Arduino's display buffer.
  for ( unsigned int OLED_x = 0; OLED_x < size_x; OLED_x++ ) {
    for ( unsigned int OLED_y = 0; OLED_y < size_y; OLED_y++ ) {
      oled.rect(
          OLED_x*(OLED_seg_width_with_border+OLED_GAP),
          OLED_y*(OLED_seg_height_with_border+OLED_GAP),
          OLED_seg_width_with_border,
          OLED_seg_height_with_border,
          WHITE,
          NORM
      );
    }
  }
  oled.display();//display the image 
}

void debugOLEDDisplay() {
  oled.display();
}

void debugOLEDDrawSegment(unsigned int _x, unsigned int _y, boolean _filled) {
  _y = OLED_y_max-1-_y; // flip vertically to match OLED shield and painting area orientation

  oled.rectFill(
      _x*(OLED_seg_width_with_border+OLED_GAP) + OLED_BORDER,
      _y*(OLED_seg_height_with_border+OLED_GAP) + OLED_BORDER,
      OLED_seg_width,
      OLED_seg_height,
      (_filled ? WHITE : BLACK),
      NORM
  );
}

void debugOLEDSetup() {
  PRINTSLN("oledSetup");
  // Before you can start using the OLED, call begin() to init
  // all of the pins and configure the OLED.
  oled.begin();
  debugOLEDInitField(OLED_x_max, OLED_y_max);
}

#else
  #define debugOLEDSetup()
  #define debugOLEDDisplay()
  #define debugOLEDDrawSegment(_x, _y, _filled)
#endif
