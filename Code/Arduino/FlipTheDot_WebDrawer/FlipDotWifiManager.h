#ifndef FlipDotWifiManager_h
#define FlipDotWifiManager_h

#include "setupSerial.h"

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h>


class FlipDotWifiManager {
    public:
      FlipDotWifiManager(const char *hostAndPortalName, bool debugEnabled = false);
      WiFiManager *bootstrapWifiManager(bool isOnDemand);
      bool setTimeout(unsigned int seconds);
      bool setupMDNS();
      void setupPortal();
      void postConnect();
      bool reconnect();
      bool setName(const char *hostAndPortalName);
      String getName();
      
    protected:
      bool _mDNSDone = false;
      const char *_hostAndPortalName;
      unsigned int _timeout = 60;
      WiFiManagerParameter *_customParamNewDeviceName;
      bool _debugEnabled = true;
      unsigned int length(const char * str);
};

#endif
