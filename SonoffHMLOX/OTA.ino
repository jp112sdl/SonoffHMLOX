void startOTAhandling() {
  DEBUG("Starte OTA-Handler...","OTA",_slInformational);
  ArduinoOTA.onStart([]() {
    DEBUG(F("Start updating"),"OTA",_slInformational);
    OTAStart = true;
  });
  ArduinoOTA.onEnd([]() {
    DEBUG("\nEnd","OTA",_slInformational);
    OTAStart = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf(".");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    OTAStart = false;
    DEBUG("Error "+String(error)+": ","OTA",_slError);
    if (error == OTA_AUTH_ERROR) DEBUG("Auth Failed","OTA",_slError);
    else if (error == OTA_BEGIN_ERROR) DEBUG("Begin Failed","OTA",_slError);
    else if (error == OTA_CONNECT_ERROR) DEBUG("Connect Failed","OTA",_slError);
    else if (error == OTA_RECEIVE_ERROR) DEBUG("Receive Failed","OTA",_slError);
    else if (error == OTA_END_ERROR) DEBUG("End Failed","OTA",_slError);
  });

  ArduinoOTA.setHostname(GlobalConfig.Hostname.c_str());
  //ArduinoOTA.setPassword((const char *)"sonoffota");
  ArduinoOTA.begin();
}
