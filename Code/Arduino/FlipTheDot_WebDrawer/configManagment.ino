bool serverHandleConfigUpdate() {
  // TODO remove this function or implement a usable interface

  unsigned int found = 0;
  unsigned int updated = 0;
  
  if ( server.method() == HTTP_POST || server.method() == HTTP_PUT || (server.method() == HTTP_GET && server.args() > 0) ) {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      found++;
      if ( server.arg(i) == "" ) {
        PRINTSLN(server.argName(i) + " is empty and therefore ignored");
        continue;
      }
      if ( server.argName(i) == "offTime" ) {
        offTime = server.arg(i).toInt();
      }
      else if ( server.argName(i) == "onTime" ) {
        onTime = server.arg(i).toInt();
      }
      else if ( server.argName(i) == "deviceName" ) {
        deviceName = server.arg(i);
      }
      else {
        PRINTSLN(server.argName(i) + " unknown");
        continue;
      }
      PRINTSLN(server.argName(i) + " updated");
      updated++;
    }
  }

  if ( found > 0 && updated > 0 ) {
    String saved = configSave() ? "true" : "false";
    String message = String("{\"found\": ") + String(found) + String(", \"updated\": ") + String(updated) + String(", \"saved\": ") + saved + String("}");
    server.send(200, "application/json", message);
    PRINTSLN(message);
    return true;
  }
  else
  {
    server.send(500, "application/json", "Nothing found to update");
    PRINTSLN("Nothing found to update");
    return true;
  }
}

bool configLoad() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    PRINTSLN("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    PRINTSLN("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    PRINTSLN("Failed to parse config file");
    return false;
  }

  if ( json.containsKey("deviceName") ) {
    deviceName = json.get<String>("deviceName");
  }
  if ( json.containsKey("offTime") ) {
    offTime = json["offTime"];
  }
  if ( json.containsKey("onTime") ) {
    onTime = json["onTime"];
  }

  // Real world application would store these values in some variables for
  // later use.

  PRINTS("Loaded deviceName: ");
  PRINTSLN(deviceName);
  PRINTS("Loaded offTime: ");
  PRINTSLN(offTime);
  PRINTS("Loaded onTime: ");
  PRINTSLN(onTime);
  return true;
}

bool configSave() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["deviceName"] = deviceName;
  json["offTime"] = offTime;
  json["onTime"] = onTime;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    PRINTSLN("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  configFile.close();
  return true;
}
