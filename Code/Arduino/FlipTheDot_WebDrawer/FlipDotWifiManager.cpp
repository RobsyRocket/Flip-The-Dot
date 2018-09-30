#include "FlipDotWifiManager.h"
#include "setup.h"

FlipDotWifiManager::FlipDotWifiManager(const char *hostAndPortalName) {
  _hostAndPortalName = hostAndPortalName;

  // maxium length of 32 due SSID length limit
  // pattern to ensure that only names can be enter which can work as names for the SSID, hostname and webserver domain
  _customParamNewDeviceName = new WiFiManagerParameter("new_device_name", "New device name", "", 32, " pattern=\"[A-Za-z]([A-Za-z0-9-]*[A-Za-z0-9])\"");
};

bool FlipDotWifiManager::setName(const char *hostAndPortalName) {
  // TODO validation needed, 1. length must be between 1 and 32, 2. pattern [A-Za-z][A-Za-z0-9-]*[A-Za-z0-9]
  if ( WiFi.hostname(hostAndPortalName) ) {
    _hostAndPortalName = hostAndPortalName;
    setupMDNS();
    return true;
  }
  return false;
};

String FlipDotWifiManager::getName() {
  return String(_hostAndPortalName);
};

WiFiManager *FlipDotWifiManager::bootstrapWifiManager(bool isOnDemand) {
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager *wifiManager = new WiFiManager();

  // add parameter to enable renaming of the device
  wifiManager->addParameter(_customParamNewDeviceName);

  if ( isOnDemand == true ) {
    wifiManager->setTimeout(_timeout);
  }

  // WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
  WiFi.mode(WIFI_STA);
  
  #if DEBUG 
    wifiManager->setDebugOutput(true);
  #else
    wifiManager->setDebugOutput(false);
  #endif
  return wifiManager;
}

void FlipDotWifiManager::setupPortal() {
  // TODO think about to implement WPS configuration too, https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/station-class.html#wps
  
  WiFiManager *wifiManager = bootstrapWifiManager(false);
  // start an access point with the specified name
  // and go into a blocking loop awaiting configuration)
  if ( !wifiManager->startConfigPortal(_hostAndPortalName) ) {
    PRINTSLN("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  postConnect();
};

bool FlipDotWifiManager::setupMDNS(){
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(_hostAndPortalName)) {
    PRINTSLN("Error setting up MDNS responder!");
    return false;
  }
  else {
    if ( !_mDNSDone ) {
      // Add service to MDNS-SD
      MDNS.addService("http", "tcp", 80);
    }
    _mDNSDone = true;
  
    PRINTSLN("mDNS responder started");
    PRINTS("Look for network device named: ");
    PRINTSLN(_hostAndPortalName);
    return true;
  }
};

bool FlipDotWifiManager::reconnect() {
  WiFiManager *wifiManager = bootstrapWifiManager(true);
  if ( !wifiManager->autoConnect(_hostAndPortalName) ) {
      PRINTSLN("failed to connect and hit timeout");
      return false;
  }
  postConnect();
  return true;
}

void FlipDotWifiManager::postConnect() {
  //if you get here you have connected to the WiFi
  PRINTSLN("connected to WiFi network");

  if ( _customParamNewDeviceName->getValueLength() > 0 ) {
    setName(_customParamNewDeviceName->getValue());
  }
  else {
    setupMDNS();
  }
}
