//
// logic to handle incomming web requests for the drawer to pass them to connected display
//

void drawerServerHandleConfig() {
  if ( server.method() == HTTP_PUT ) {
    PRINTSLN("Drawer config request as PUT");
    if ( !drawerConfigUpdate() ) {
      PRINTSLN("Failed");
      setNoCacheHeaders();
      server.send (500, "application/json", "{\"success\": false, \"message\": \"Config could not be saved.\"}");
      return;
    }
    PRINTSLN("Success");
  }
  drawerConfigLoad();
}

bool drawerConfigUpdate() {
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));

  if (!root.success()) {
    PRINTSLN("Failed to parse incomming JSON");
    return false;
  }

  File configFile = SPIFFS.open("/drawer/config.json", "w");
  if (!configFile) {
    PRINTSLN("Failed to open drawer/config.json file for writing");
    return false;
  }

  root.printTo(configFile);
  configFile.close();
  return true;
}

bool drawerConfigLoad() {
  return loadFromSpiffs("/drawer/config.json");
}

/**
 * handle the incomming request, pass them to the display logic and send the response
 */
void drawerServerHandleCommand() {
    // check if the segmentNo is not in the allowed range 
    if ( !server.hasArg("commands")  )
    {
        setNoCacheHeaders();
        server.send ( 400, "application/json", "{\"success\": \"false\", message: \"No/invalid value for parameter \\\"commands\\\" provided.\"}\n");
        return;
    }

    const String cmdValue = server.arg("commands");

    PRINTSLN(cmdValue);
    String cmdSegmentValue = "";
    int cmdSegmentIndex = 0;

    String cmdSegmentPartValue = "";
    int cmdSegmentPartIndex = 0;
    String c, r, is_high;
    
    while ( (cmdSegmentValue = getPart(cmdValue, ' ', cmdSegmentIndex)) != "" )
    {
        PRINTS(cmdSegmentIndex);
        PRINTS(": ");
        PRINTSLN(cmdSegmentValue);

        cmdSegmentPartIndex = 0;

        checkAndExecuteCommand(cmdSegmentValue);
        cmdSegmentIndex++;
    }
    // TODO remove alternativ debugging code
    // mled.display();

    setNoCacheHeaders();
    server.send ( 200, "application/json", "{\"success\": true, \"message\": \"Commands processed.\", \"amount\": "+ String(cmdSegmentIndex) +"}\n");
}

/**
 * utility to split the incomming command into pieces
 */
String getPart(const String &data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean checkAndExecuteCommand(const String &command)
{
  String c, r, high;
  unsigned int c_int, r_int;
  high = getPart(command, 'x', 2);
  r = getPart(command, 'x', 1);
  c = getPart(command, 'x', 0);

  PRINTS("c: ");
  PRINTSLN(c);

  PRINTS("r: ");
  PRINTSLN(r);

  PRINTS("high: ");
  PRINTSLN(high);

  if ( c != "" && (c_int = c.toInt()) > 0 )
  {
    if ( r != "" && (r_int = r.toInt()) > 0 )
    {
      if ( high != "" && (high.toInt() == 0 || high.toInt() == 1) )
      {
        PRINTSLN("sending to display: " + c + "x" + r + "x" + high);

        SerialDisplay.println(c + "x" + r + "x" + high);

        // TODO remove alternativ debugging code
        // Commands a 1 based, leds are 0 based, leds limited by 8 in each direction
        /*
        if ( c_int < 9 && r_int < 9 ) {
          // mled.dot(c_int-1, r_int-1, high.toInt()==1);
        }
        */
        return true;
      }
    }
  }
  return false;
}
