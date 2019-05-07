// TODO overall documentation, code review and cleanup

// Set to 0 to disable debug output, set to 1 for standard (hardware) Serial, or set to 2 for SoftwareSerial.
#define DEBUG 0
#include "setupSerial.h"


// Set to 1 if a Lolin(Wemos) OLED shield is connected as alternative output. Updating the display will slow down the response rate.
#define DEBUG_OLED 0
// go to this file to configure the OLED device properties and layout settings
#include "debugOLED.h"


#include "FlipDotWifiManager.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "FS.h"

String deviceName = "FlipDot";
ESP8266WebServer server(80);  //port 80
#if defined(DEBUG) && DEBUG
FlipDotWifiManager wifiManager(deviceName.c_str(), true);
#else
FlipDotWifiManager wifiManager(deviceName.c_str(), false);
#endif


#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);


// TODO think about if this is needed or can be solved differently
int led_pin = LED_BUILTIN;



void setup() {
  /* switch on led */
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  SerialDisplay.begin(115200);
  // begin SerialDebug only when it is not declared with a define, otherwise it will be equal to SerialDisplay, which got begin already called
  #if DEBUG == 2
    SerialDebug.begin(115200);
    PRINTSLN("");
  #endif

  delay(1000);

  debugOLEDSetup();
  
   // starting SPIFFS thrings
  PRINTSLN("Mounting FS...");

  if (!SPIFFS.begin()) {
    PRINTSLN("Failed to mount file system");
    return;
  }

  if (!configLoad()) {
    PRINTSLN("Failed to load config");
  } else {
    PRINTSLN("Config loaded");
  }

  wifiManager.setName(deviceName.c_str());
  wifiManager.reconnect();

  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);

  // check if device name got changed within Wifi Manager logic
  if ( deviceName != wifiManager.getName() ) {
    // if yes, then read back the new value and update the configuration
    deviceName = wifiManager.getName();
    configSave();
  }
  
  
  // setup server things
  server.on("/", HTTP_GET, [](){
    server.sendHeader("Location", String("/drawer.html"), true);
    server.send(302, "text/plain", "");
  });
  server.on("/drawer/commands", drawerServerHandleCommand);
  server.on("/drawer/config", drawerServerHandleConfig);
  server.on("/config", serverHandleConfigUpdate);
  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/status", [](){
    server.send(200, "text/plain", getRequestInformationOutput());
  });
  
  server.onNotFound(handleOther);
  server.begin();
  PRINTSLN("HTTP server started");
}

void loop() {
  // satisfy ESP8266 background processes
  yield();
  // HTTP
  webSocket.loop();
  server.handleClient();
}
