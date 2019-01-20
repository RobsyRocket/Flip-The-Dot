void setNoCacheHeaders() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Expires", "-1");
}

void setStaticCacheHeaders() {
    server.sendHeader("Cache-Control", "max-age=1314000, public");
}

void closeClientConnection() {
  server.sendContent(""); // *** END 1/2 ***
  server.client().stop(); // *** END 2/2 *** // Stop is needed because we sent no content length
}

String getRequestInformationOutput()
{
    String message = "";
    if (server.client().localIP() == WiFi.softAPIP()) {
        message += String("Connected through soft AP: ") + WiFi.softAPSSID() + "\n";
    } else {
        message += "Connected through WiFi network: " + WiFi.SSID() + "\n";
    }
    message += "Client IP: " + toStringIp(server.client().remoteIP()) + "\n";
    message += "Server IP: " + toStringIp(server.client().localIP()) + "\n";
    message += "Server Name: " + WiFi.hostname() + "\n";
  
    message += "Requested URI: ";
    message += server.uri();
    message += "\nMethod: ";
    if ( server.method() == HTTP_GET) { message += "GET"; }
    else if ( server.method() == HTTP_POST ) { message += "POST"; }
    else if ( server.method() == HTTP_PUT ) { message += "PUT"; }
    else if ( server.method() == HTTP_DELETE ) { message += "DELETE"; }
    else { message += "other"; }
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }

    #if defined(DEBUG) && DEBUG
      message += "Free Heap: " + String(ESP.getFreeHeap()) + "\n";
      message += "GPIO: " + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16))) + "\n";
    #endif

    return message;
}

/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
