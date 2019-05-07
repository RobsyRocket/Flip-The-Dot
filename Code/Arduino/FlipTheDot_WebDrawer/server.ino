//tell the client what to do with data
bool loadFromSpiffs(String path)
{
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm"; //this is where index.htm is created

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  else if (path.endsWith(".json")) dataType = "application/json";
  

  File dataFile = SPIFFS.open(path.c_str(), "r");   //open file to read
  if (!dataFile)  //unsuccesful open
  {
      PRINTS("Don't know this command and it's not a file in SPIFFS : ");
      PRINTSLN(path);
    return false;
  }
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {}    //a lot happening here

  dataFile.close();

  return true; //shouldn't always return true, Added false above
}
//-----------------------------------------------------------------------------------------
void handleOther() {   
  if (loadFromSpiffs(server.uri())) return;   //gotcha - it's a file in SPIFFS

  PRINTSLN("handleOther 404");

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(404, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.

  server.sendContent("Not Found\n\n");           //or not...
  server.sendContent(getRequestInformationOutput());
  closeClientConnection();
}

bool handleCommand() {
  if ( server.method() == HTTP_POST || server.method() == HTTP_PUT || (server.method() == HTTP_GET && server.args() > 0) ) {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      if ( server.argName(i) == "commands" ) {
        PRINTSLN(server.arg(i));
        server.send(200, "application/json", "{\"success\": true}");
        return true;
      }
    }
  }
  server.send(401, "application/json", "{\"success\": false}");
  return true;
}

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  // Do something with the data from the client
  if(type == WStype_TEXT){
    if ( checkAndExecuteCommand((char*)payload) ) {
      // update OLED instantly
      debugOLEDDisplay();
    }
  }
}
