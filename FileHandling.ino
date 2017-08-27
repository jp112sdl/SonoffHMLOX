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
          ((json["ip"]).as<String>()).toCharArray(SonoffNetConfig.ip, IPSIZE);
          ((json["netmask"]).as<String>()).toCharArray(SonoffNetConfig.netmask, IPSIZE);
          ((json["gw"]).as<String>()).toCharArray(SonoffNetConfig.gw, IPSIZE);
          ((json["ccuip"]).as<String>()).toCharArray(GlobalConfig.ccuIP, IPSIZE);
          ((json["sonoff"]).as<String>()).toCharArray(GlobalConfig.DeviceName, VARIABLESIZE);
          //((json["loxusername"]).as<String>()).toCharArray(LoxoneConfig.Username, VARIABLESIZE);
          //((json["loxpassword"]).as<String>()).toCharArray(LoxoneConfig.Password, VARIABLESIZE);
          ((json["loxudpport"]).as<String>()).toCharArray(LoxoneConfig.UDPPort, 10);
          ((json["powervariable"]).as<String>()).toCharArray(HomeMaticConfig.PowerVariableName, VARIABLESIZE);
          GlobalConfig.MeasureInterval = json["measureinterval"];
          if (GlobalConfig.MeasureInterval == 0)
            GlobalConfig.MeasureInterval = 60;

          GlobalConfig.BackendType = json["backendtype"];
          GlobalConfig.restoreOldRelayState = json["restoreOldState"];
          GlobalConfig.SonoffModel = json["sonoffmodel"];
          GlobalConfig.Hostname = "Sonoff-" + String(GlobalConfig.DeviceName);
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
  json["ip"] = SonoffNetConfig.ip;
  json["netmask"] = SonoffNetConfig.netmask;
  json["gw"] = SonoffNetConfig.gw;
  json["ccuip"] = GlobalConfig.ccuIP;
  json["sonoff"] = GlobalConfig.DeviceName;
  json["restoreOldState"] = GlobalConfig.restoreOldRelayState;
  json["backendtype"] = GlobalConfig.BackendType;
  //json["loxusername"] = LoxoneConfig.Username;
  //json["loxpassword"] = LoxoneConfig.Password;
  json["loxudpport"] = LoxoneConfig.UDPPort;
  json["powervariable"] = HomeMaticConfig.PowerVariableName;
  json["measureinterval"] = GlobalConfig.MeasureInterval;
  if (GlobalConfig.MeasureInterval == 0) GlobalConfig.MeasureInterval = 60;
  json["sonoffmodel"] = GlobalConfig.SonoffModel;

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

