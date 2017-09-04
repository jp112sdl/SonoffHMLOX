#define JSONCONFIG_IP                     "ip"
#define JSONCONFIG_NETMASK                "netmask"
#define JSONCONFIG_GW                     "gw"
#define JSONCONFIG_CCUIP                  "ccuip"
#define JSONCONFIG_SONOFF                 "sonoff"
#define JSONCONFIG_LOXUDPPORT             "loxudpport"
#define JSONCONFIG_LOXUSERNAME            "loxusername"
#define JSONCONFIG_LOXPASSWORD            "loxpassword"
#define JSONCONFIG_POWERVARIABLE          "powervariable"
#define JSONCONFIG_MEASUREINTERVAL        "measureinterval"
#define JSONCONFIG_BACKENDTYPE            "backendtype"
#define JSONCONFIG_RESTOREOLDSTATE        "restoreOldState"
#define JSONCONFIG_LEDENABLED             "ledEnabled"
#define JSONCONFIG_SONOFFMODEL            "sonoffmodel"
#define JSONCONFIG_HLW_CURRENTMULTIPLIER  "hlw_currentmultiplier"
#define JSONCONFIG_HLW_VOLTAGEMULTIPLIER  "hlw_voltagemultiplier"
#define JSONCONFIG_HLW_POWERMULTIPLIER    "hlw_powermultiplier"

bool loadSystemConfig() {
  Serial.println(F("loadSystemConfig mounting FS..."));
  if (SPIFFS.begin()) {
    Serial.println(F("loadSystemConfig mounted file system"));
    if (SPIFFS.exists("/" + configJsonFile)) {
      Serial.println(F("loadSystemConfig reading config file"));
      File configFile = SPIFFS.open("/" + configJsonFile, "r");
      if (configFile) {
        Serial.println(F("loadSystemConfig opened config file"));
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        Serial.println("Content of JSON Config-File: /" + configJsonFile);
        json.printTo(Serial);
        Serial.println();
        if (json.success()) {
          Serial.println("\nJSON OK");
          ((json[JSONCONFIG_IP]).as<String>()).toCharArray(SonoffNetConfig.ip, IPSIZE);
          ((json[JSONCONFIG_NETMASK]).as<String>()).toCharArray(SonoffNetConfig.netmask, IPSIZE);
          ((json[JSONCONFIG_GW]).as<String>()).toCharArray(SonoffNetConfig.gw, IPSIZE);
          ((json[JSONCONFIG_CCUIP]).as<String>()).toCharArray(GlobalConfig.ccuIP, IPSIZE);
          ((json[JSONCONFIG_SONOFF]).as<String>()).toCharArray(GlobalConfig.DeviceName, VARIABLESIZE);
          //((json[JSONCONFIG_LOXUSERNAME]).as<String>()).toCharArray(LoxoneConfig.Username, VARIABLESIZE);
          //((json[JSONCONFIG_LOXPASSWORD]).as<String>()).toCharArray(LoxoneConfig.Password, VARIABLESIZE);
          ((json[JSONCONFIG_LOXUDPPORT]).as<String>()).toCharArray(LoxoneConfig.UDPPort, 10);
          ((json[JSONCONFIG_POWERVARIABLE]).as<String>()).toCharArray(HomeMaticConfig.PowerVariableName, VARIABLESIZE);
          GlobalConfig.MeasureInterval = json[JSONCONFIG_MEASUREINTERVAL];
          if (GlobalConfig.MeasureInterval == 0)
            GlobalConfig.MeasureInterval = 60;

          GlobalConfig.BackendType = json[JSONCONFIG_BACKENDTYPE];
          GlobalConfig.restoreOldRelayState = json[JSONCONFIG_RESTOREOLDSTATE];
          GlobalConfig.LEDEnabled = json[JSONCONFIG_LEDENABLED];
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

        } else {
          Serial.println(F("\nloadSystemConfig ERROR loading config"));
        }
      }
      return true;
    } else {
      Serial.println("/" + configJsonFile + " not found.");
      return false;
    }
    SPIFFS.end();
  } else {
    Serial.println(F("loadSystemConfig failed to mount FS"));
    return false;
  }
}

bool saveSystemConfig() {
  SPIFFS.begin();
  Serial.println(F("saving config"));
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json[JSONCONFIG_IP] = SonoffNetConfig.ip;
  json[JSONCONFIG_NETMASK] = SonoffNetConfig.netmask;
  json[JSONCONFIG_GW] = SonoffNetConfig.gw;
  json[JSONCONFIG_CCUIP] = GlobalConfig.ccuIP;
  json[JSONCONFIG_SONOFF] = GlobalConfig.DeviceName;
  json[JSONCONFIG_RESTOREOLDSTATE] = GlobalConfig.restoreOldRelayState;
  json[JSONCONFIG_LEDENABLED] = GlobalConfig.LEDEnabled;
  json[JSONCONFIG_BACKENDTYPE] = GlobalConfig.BackendType;
  //json[JSONCONFIG_LOXUSERNAME] = LoxoneConfig.Username;
  //json[JSONCONFIG_LOXPASSWORD] = LoxoneConfig.Password;
  json[JSONCONFIG_LOXUDPPORT] = LoxoneConfig.UDPPort;
  json[JSONCONFIG_POWERVARIABLE] = HomeMaticConfig.PowerVariableName;
  json[JSONCONFIG_MEASUREINTERVAL] = GlobalConfig.MeasureInterval;
  if (GlobalConfig.MeasureInterval == 0) GlobalConfig.MeasureInterval = 60;
  json[JSONCONFIG_SONOFFMODEL] = GlobalConfig.SonoffModel;

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    json[JSONCONFIG_HLW_CURRENTMULTIPLIER] = HLW8012Calibration.CurrentMultiplier;
    json[JSONCONFIG_HLW_VOLTAGEMULTIPLIER] = HLW8012Calibration.VoltageMultiplier;
    json[JSONCONFIG_HLW_POWERMULTIPLIER] = HLW8012Calibration.PowerMultiplier;
  }

  SPIFFS.remove("/" + configJsonFile);
  File configFile = SPIFFS.open("/" + configJsonFile, "w");
  if (!configFile) {
    Serial.println(F("failed to open config file for writing"));
    return false;
  }

  json.printTo(Serial);
  Serial.println();
  json.printTo(configFile);
  configFile.close();
  SPIFFS.end();
  return true;
}

void setLastState(bool state) {
  GlobalConfig.lastRelayState = state;
  if (GlobalConfig.restoreOldRelayState) {
    if (SPIFFS.begin()) {
      Serial.println(F("setLastState mounted file system"));
      //SPIFFS.remove("/" + lastStateFilename);
      File setLastStateFile = SPIFFS.open("/" + lastRelayStateFilename, "w");
      setLastStateFile.print(state);
      setLastStateFile.close();
      SPIFFS.end();
      Serial.println("setLastState (" + String(state) + ") saved.");
    } else {
      Serial.println(F("setLastState SPIFFS mount fail!"));
    }
  }
}

bool getLastState() {
  if (GlobalConfig.restoreOldRelayState) {
    if (SPIFFS.begin()) {
      Serial.println(F("getLastState mounted file system"));
      if (SPIFFS.exists("/" + lastRelayStateFilename)) {
        Serial.println(lastRelayStateFilename + " existiert");
        File lastStateFile = SPIFFS.open("/" + lastRelayStateFilename, "r");
        bool bLastState = false;
        if (lastStateFile && lastStateFile.size()) {
          String content = String(char(lastStateFile.read()));
          Serial.println("getLastState FileContent = " + content);
          bLastState = (content == "1");
        }
        SPIFFS.end();
        return bLastState;
      } else {
        Serial.println(lastRelayStateFilename + " existiert nicht");
      }
    } else {
      Serial.println(F("getLastState SPIFFS mount fail!"));
      false;
    }
  } else {
    return false;
  }
}

void setBootConfigMode() {
  if (SPIFFS.begin()) {
    Serial.println(F("setBootConfigMode mounted file system"));
    if (!SPIFFS.exists("/" + bootConfigModeFilename)) {
      File bootConfigModeFile = SPIFFS.open("/" + bootConfigModeFilename, "w");
      bootConfigModeFile.print("0");
      bootConfigModeFile.close();
      SPIFFS.end();
      Serial.println(F("Boot to ConfigMode requested. Restarting..."));
      WebServer.send(200, "text/plain", F("<state>enableBootConfigMode - Rebooting</state>"));
      delay(500);
      ESP.restart();
    } else {
      WebServer.send(200, "text/plain", F("<state>enableBootConfigMode - FAILED!</state>"));
      SPIFFS.end();
    }
  }
}

