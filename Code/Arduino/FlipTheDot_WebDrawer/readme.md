WebDrawer is a kind of IoT extension for the Flip The Dot project and enables interactive drawing onto the display(s) via network. This is done by adding a second microcontroller to the setup which provide network accessibilty and serves the drawing art board for your web browser.

The additional controller is a ESP8266 which can be connected to an existing WiFi network or open its own if needed to allow configuration and/or independent operation. The connection and control of the display(s) is done via a serial communication link with the Flip The Dot driver board.

Customization of the browser interface can be done by editing the files in the "data" directory, which contain all publicly available files. To upload these files to the ESP8266, it is needed to use the menu "Tools > ESP8266 Sketch Data Upload" in the Arduino IDE or a alternative way. The menu and feature can be enabled by performing the modification described in https://github.com/esp8266/arduino-esp8266fs-plugin .