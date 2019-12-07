#define JSONCONFIG_IP                     "ip"
#define JSONCONFIG_NETMASK                "netmask"
#define JSONCONFIG_GW                     "gw"
#define JSONCONFIG_CCUIP                  "ccuip"
#define JSONCONFIG_SONOFF                 "sonoff"
#define JSONCONFIG_LOXUDPPORT              "loxudpport"
#define JSONCONFIG_LOXUSERNAME             "loxusername"
#define JSONCONFIG_LOXPASSWORD             "loxpassword"
#define JSONCONFIG_HMPOWERVARIABLE         "powervariable"
#define JSONCONFIG_HMENERGYCOUNTERVARIABLE "ecvariable"
#define JSONCONFIG_MEASUREINTERVAL         "measureinterval"
#define JSONCONFIG_BACKENDTYPE             "backendtype"
#define JSONCONFIG_RESTOREOLDSTATE         "restoreOldState"
#define JSONCONFIG_LEDDISABLED             "ledDisabled"
#define JSONCONFIG_SONOFFMODEL             "sonoffmodel"
#define JSONCONFIG_HLW_CURRENTMULTIPLIER   "hlw_currentmultiplier"
#define JSONCONFIG_HLW_VOLTAGEMULTIPLIER   "hlw_voltagemultiplier"
#define JSONCONFIG_HLW_POWERMULTIPLIER     "hlw_powermultiplier"
#define JSONCFONIG_GPIO14MODE              "gpio14mode"
#define JSONCFONIG_GPIO14ASSENDER          "gpio14assender"
#define JSONCONFIG_LOADECONBOOT            "loadEcOnBoot"

bool loadSystemConfig() {
  DEBUG(F("loadSystemConfig mounting FS..."), "loadSystemConfig()", _slInformational);
  if (SPIFFS.begin()) {
    DEBUG(F("loadSystemConfig mounted file system"), "loadSystemConfig()", _slInformational);
    if (SPIFFS.exists("/" + configJsonFile)) {
      DEBUG(F("loadSystemConfig reading config file"), "loadSystemConfig()", _slInformational);
      File configFile = SPIFFS.open("/" + configJsonFile, "r");
      if (configFile) {
        DEBUG(F("loadSystemConfig opened config file"), "loadSystemConfig()", _slInformational);
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, buf.get());
        if (error) {
          DEBUG(F("loadSystemConfig JSON DeserializationError"), "loadSystemConfig()", _slInformational);
          return false;
        }
        JsonObject json = doc.as<JsonObject>();
        DEBUG("Content of JSON Config-File: /" + configJsonFile, "loadSystemConfig()", _slInformational);
#ifdef SERIALDEBUG
        serializeJson(doc, Serial);
        Serial.println();
#endif
        DEBUG("\nJSON OK", "loadSystemConfig()", _slInformational);
        ((json[JSONCONFIG_IP]).as<String>()).toCharArray(SonoffNetConfig.ip, IPSIZE);
        ((json[JSONCONFIG_NETMASK]).as<String>()).toCharArray(SonoffNetConfig.netmask, IPSIZE);
        ((json[JSONCONFIG_GW]).as<String>()).toCharArray(SonoffNetConfig.gw, IPSIZE);
        ((json[JSONCONFIG_CCUIP]).as<String>()).toCharArray(GlobalConfig.ccuIP, IPSIZE);
        ((json[JSONCONFIG_SONOFF]).as<String>()).toCharArray(GlobalConfig.DeviceName, VARIABLESIZE);

        //((json[JSONCONFIG_LOXUSERNAME]).as<String>()).toCharArray(LoxoneConfig.Username, VARIABLESIZE);
        //((json[JSONCONFIG_LOXPASSWORD]).as<String>()).toCharArray(LoxoneConfig.Password, VARIABLESIZE);
        ((json[JSONCONFIG_LOXUDPPORT]).as<String>()).toCharArray(LoxoneConfig.UDPPort, 10);
        ((json[JSONCONFIG_HMPOWERVARIABLE]).as<String>()).toCharArray(HomeMaticConfig.PowerVariableName, VARIABLESIZE);
        ((json[JSONCONFIG_HMENERGYCOUNTERVARIABLE]).as<String>()).toCharArray(HomeMaticConfig.EnergyCounterVariableName, VARIABLESIZE);
        GlobalConfig.MeasureInterval = json[JSONCONFIG_MEASUREINTERVAL];
        if (GlobalConfig.MeasureInterval == 0)
          GlobalConfig.MeasureInterval = 60;

        GlobalConfig.BackendType = json[JSONCONFIG_BACKENDTYPE];
        GlobalConfig.GPIO14Mode = json[JSONCFONIG_GPIO14MODE];
        GlobalConfig.restoreOldRelayState = json[JSONCONFIG_RESTOREOLDSTATE];
        GlobalConfig.LEDDisabled = json[JSONCONFIG_LEDDISABLED];
        GlobalConfig.loadEcOnBoot = json[JSONCONFIG_LOADECONBOOT];
        GlobalConfig.GPIO14asSender = json[JSONCFONIG_GPIO14ASSENDER];
        GlobalConfig.SonoffModel = json[JSONCONFIG_SONOFFMODEL];
        GlobalConfig.Hostname = "Sonoff-" + String(GlobalConfig.DeviceName);

        if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
          float fVal = (json[JSONCONFIG_HLW_CURRENTMULTIPLIER]).as<float>();
          if (fVal > 0.0) {
            if (HLW8012Calibration.CurrentMultiplier != fVal) {
              HLW8012Calibration.CurrentMultiplier = fVal;
            }
          }
          fVal = (json[JSONCONFIG_HLW_VOLTAGEMULTIPLIER]).as<float>();
          if (fVal > 0.0) {
            if (HLW8012Calibration.VoltageMultiplier != fVal) {
              HLW8012Calibration.VoltageMultiplier =  fVal;
            }
          }
          fVal = (json[JSONCONFIG_HLW_POWERMULTIPLIER]).as<float>();
          if (fVal > 0.0) {
            if (HLW8012Calibration.PowerMultiplier != fVal) {
              HLW8012Calibration.PowerMultiplier =  fVal;
            }
          }
        }
      }
      return true;
    } else {
      DEBUG("/" + configJsonFile + " not found.", "loadSystemConfig()", _slInformational);
      return false;
    }
    SPIFFS.end();
  } else {
    DEBUG(F("loadSystemConfig failed to mount FS"), "loadSystemConfig()", _slCritical);
    return false;
  }
}

bool saveSystemConfig() {
  SPIFFS.begin();
  DEBUG(F("saving config"), "saveSystemConfig()", _slInformational);
  StaticJsonDocument<1024> doc;
  JsonObject json = doc.to<JsonObject>();
  json[JSONCONFIG_IP] = SonoffNetConfig.ip;
  json[JSONCONFIG_NETMASK] = SonoffNetConfig.netmask;
  json[JSONCONFIG_GW] = SonoffNetConfig.gw;
  json[JSONCONFIG_CCUIP] = GlobalConfig.ccuIP;
  json[JSONCONFIG_SONOFF] = GlobalConfig.DeviceName;
  json[JSONCONFIG_RESTOREOLDSTATE] = GlobalConfig.restoreOldRelayState;
  json[JSONCONFIG_LEDDISABLED] = GlobalConfig.LEDDisabled;
  json[JSONCONFIG_LOADECONBOOT] = GlobalConfig.loadEcOnBoot;
  json[JSONCONFIG_BACKENDTYPE] = GlobalConfig.BackendType;
  //json[JSONCONFIG_LOXUSERNAME] = LoxoneConfig.Username;
  //json[JSONCONFIG_LOXPASSWORD] = LoxoneConfig.Password;
  json[JSONCONFIG_LOXUDPPORT] = LoxoneConfig.UDPPort;
  json[JSONCONFIG_HMPOWERVARIABLE] = HomeMaticConfig.PowerVariableName;
  json[JSONCONFIG_HMENERGYCOUNTERVARIABLE] = HomeMaticConfig.EnergyCounterVariableName;
  json[JSONCONFIG_MEASUREINTERVAL] = GlobalConfig.MeasureInterval;
  if (GlobalConfig.MeasureInterval == 0) GlobalConfig.MeasureInterval = 60;
  json[JSONCONFIG_SONOFFMODEL] = GlobalConfig.SonoffModel;
  json[JSONCFONIG_GPIO14MODE] = GlobalConfig.GPIO14Mode;
  json[JSONCFONIG_GPIO14ASSENDER] = GlobalConfig.GPIO14asSender;

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    json[JSONCONFIG_HLW_CURRENTMULTIPLIER] = HLW8012Calibration.CurrentMultiplier;
    json[JSONCONFIG_HLW_VOLTAGEMULTIPLIER] = HLW8012Calibration.VoltageMultiplier;
    json[JSONCONFIG_HLW_POWERMULTIPLIER] = HLW8012Calibration.PowerMultiplier;
  }

  SPIFFS.remove("/" + configJsonFile);
  File configFile = SPIFFS.open("/" + configJsonFile, "w");
  if (!configFile) {
    DEBUG(F("failed to open config file for writing"), "saveSystemConfig()", _slCritical);
    return false;
  }

#ifdef SERIALDEBUG
  serializeJson(doc, Serial);
  Serial.println();
#endif
  serializeJson(doc, configFile);
  configFile.close();
  SPIFFS.end();
  return true;
}

void setLastRelayState(bool state) {
  GlobalConfig.lastRelayState = state;
  if (GlobalConfig.restoreOldRelayState == RelayStateOnBoot_LAST) {
    if (SPIFFS.begin()) {
      DEBUG(F("setLastState mounted file system"), "setLastState()", _slInformational);
      //SPIFFS.remove("/" + lastStateFilename);
      File setLastStateFile = SPIFFS.open("/" + lastRelayStateFilename, "w");
      setLastStateFile.print(state);
      setLastStateFile.close();
      SPIFFS.end();
      DEBUG("setLastState (" + String(state) + ") saved.", "setLastState()", _slInformational);
    } else {
      DEBUG(F("setLastState SPIFFS mount fail!"), "setLastState()", _slCritical);
    }
  }
}

bool getLastRelayState() {
  if (GlobalConfig.restoreOldRelayState == RelayStateOnBoot_LAST) {
    if (SPIFFS.begin()) {
      DEBUG(F("getLastState mounted file system"), "getLastState()", _slInformational);
      if (SPIFFS.exists("/" + lastRelayStateFilename)) {
        DEBUG(lastRelayStateFilename + " existiert", "getLastState()", _slInformational);
        File lastStateFile = SPIFFS.open("/" + lastRelayStateFilename, "r");
        bool bLastState = false;
        if (lastStateFile && lastStateFile.size()) {
          String content = String(char(lastStateFile.read()));
          DEBUG("getLastState FileContent = " + content, "getLastState()", _slInformational);
          bLastState = (content == "1");
        }
        SPIFFS.end();
        return bLastState;
      } else {
        DEBUG(lastRelayStateFilename + " existiert nicht", "getLastState()", _slInformational);
      }
    } else {
      DEBUG(F("getLastState SPIFFS mount fail!"), "getLastState()", _slCritical);
      return false;
    }
  } else {
    return false;
  }
  return false;
}

void setBootConfigMode() {
  if (SPIFFS.begin()) {
    DEBUG(F("setBootConfigMode mounted file system"), "setBootConfigMode()", _slInformational);
    if (!SPIFFS.exists("/" + bootConfigModeFilename)) {
      File bootConfigModeFile = SPIFFS.open("/" + bootConfigModeFilename, "w");
      bootConfigModeFile.print("0");
      bootConfigModeFile.close();
      SPIFFS.end();
      DEBUG(F("Boot to ConfigMode requested. Restarting..."), "setBootConfigMode()", _slInformational);
      WebServer.send(200, "text/plain", F("<state>enableBootConfigMode - Rebooting</state>"));
      delay(500);
      ESP.restart();
    } else {
      WebServer.send(200, "text/plain", F("<state>enableBootConfigMode - FAILED!</state>"));
      SPIFFS.end();
    }
  }
}
