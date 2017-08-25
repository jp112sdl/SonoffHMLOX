void startOTAhandling() {
  Serial.println("Starte OTA-Handler...");
  ArduinoOTA.onStart([]() {
    Serial.println(F("Start updating"));
    OTAStart = true;
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    OTAStart = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf(".");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    OTAStart = false;
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  String Hostname = "Sonoff-OTA-" + WiFi.macAddress();
  ArduinoOTA.setHostname(Hostname.c_str());
  //ArduinoOTA.setPassword((const char *)"sonoffota");
  ArduinoOTA.begin();
}
