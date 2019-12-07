bool doWifiConnect() {
  String _ssid = WiFi.SSID();
  String _psk = WiFi.psk();

  String _pskMask = "";
  for (uint8_t i = 0; i < _psk.length(); i++) {
    _pskMask += "*";
  }
  DEBUG("ssid = " + _ssid + ", psk = " + _pskMask);


  const char* ipStr = SonoffNetConfig.ip; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
  const char* netmaskStr = SonoffNetConfig.netmask; byte netmaskBytes[4]; parseBytes(netmaskStr, '.', netmaskBytes, 4, 10);
  const char* gwStr = SonoffNetConfig.gw; byte gwBytes[4]; parseBytes(gwStr, '.', gwBytes, 4, 10);

  if (!startWifiManager && _ssid != "" && _psk != "" ) {
    DEBUG(F("Connecting WLAN the classic way..."));
    WiFi.mode(WIFI_STA);
    WiFi.hostname(GlobalConfig.Hostname);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setAutoReconnect(true);
    if (String(SonoffNetConfig.ip) != "0.0.0.0") {
      WiFi.config(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]));
      ETS_UART_INTR_DISABLE();
      wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();
    }
    WiFi.begin(_ssid.c_str(), _psk.c_str());
    int waitCounter = 0;

    while (WiFi.status() != WL_CONNECTED) {
      waitCounter++;
      Serial.print(".");
      digitalWrite(LEDPinSwitch, (!(digitalRead(LEDPinSwitch))));
      digitalWrite(LEDPinPow, (!(digitalRead(LEDPinPow))));
      digitalWrite(LEDPinObi, (!(digitalRead(LEDPinObi))));

      if (waitCounter == 30) {
        return false;
      }
      delay(500);
    }

    DEBUG("Wifi Connected");
    WiFiConnected = true;
    return true;
  } else {
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(wifiManagerDebugOutput);
    digitalWrite(LEDPinSwitch, LOW);
    digitalWrite(LEDPinPow, HIGH);
    digitalWrite(LEDPinObi, HIGH);
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    WiFiManagerParameter custom_ccuip("ccu", "IP der CCU2", GlobalConfig.ccuIP, IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    //WiFiManagerParameter custom_loxusername("loxusername", "Loxone Username", "", VARIABLESIZE);
    //WiFiManagerParameter custom_loxpassword("loxpassword", "Loxone Password", "", VARIABLESIZE,4);
    WiFiManagerParameter custom_loxudpport("loxudpport", "Loxone UDP Port", LoxoneConfig.UDPPort, 10, 0, "pattern='[0-9]{1,5}'");
    WiFiManagerParameter custom_sonoffname("sonoff", "Sonoff Ger&auml;tename", GlobalConfig.DeviceName, VARIABLESIZE, 0, "pattern='[A-Za-z0-9_ -]+'");

    String strRestoreOldState = "";
    switch (GlobalConfig.restoreOldRelayState) {
      case RelayStateOnBoot_OFF:
        strRestoreOldState = F("<option selected value='0'>Aus</option><option value='1'>letzter Zustand</option><option value='2'>Ein</option>");
        break;
      case RelayStateOnBoot_LAST:
        strRestoreOldState = F("<option value='0'>Aus</option><option selected value='1'>letzter Zustand</option><option value='2'>Ein</option>");
        break;
      case RelayStateOnBoot_ON:
        strRestoreOldState = F("<option value='0'>Aus</option><option value='1'>letzter Zustand</option><option selected value='2'>Ein</option>");
        break;
      default:
        strRestoreOldState = F("<option selected value='0'>Aus</option><option value='1'>letzter Zustand</option><option value='2'>Ein</option>");
        break;
    }
    WiFiManagerParameter custom_restorestate("restorestate", "Schaltzustand bei Boot", "", 8, 2, strRestoreOldState.c_str());

    WiFiManagerParameter custom_cbleddisabled("leddisabled_switch", "LED deaktiviert: ", (GlobalConfig.LEDDisabled) ? "1" : "0", 8, 1);

    WiFiManagerParameter custom_powervariablename("hmpowervariable_pow", "Variable f&uuml;r Leistung", HomeMaticConfig.PowerVariableName, VARIABLESIZE, 0, "pattern='[A-Za-z0-9_ -]+'");
    WiFiManagerParameter custom_ecvariablename("hmecvariable_pow", "Variable f&uuml;r Energiez&auml;hler", HomeMaticConfig.EnergyCounterVariableName, VARIABLESIZE, 0, "pattern='[A-Za-z0-9_ -]+'");

    String del = String(GlobalConfig.MeasureInterval);
    char delBuf[8];
    del.toCharArray(delBuf, 8);
    WiFiManagerParameter custom_powermeasureinterval("custom_powermeasureinterval_pow", "Messintervall", delBuf, 8, 0, "pattern='[0-9]{1,4}'");

    String backend = "";
    switch (GlobalConfig.BackendType) {
      case BackendType_HomeMatic:
        backend = F("<option selected value='0'>HomeMatic</option><option value='1'>Loxone</option>");
        break;
      case BackendType_Loxone:
        backend = F("<option value='0'>HomeMatic</option><option selected value='1'>Loxone</option>");
        break;
      default:
        backend = F("<option value='0'>HomeMatic</option><option value='1'>Loxone</option>");
        break;
    }
    WiFiManagerParameter custom_backendtype("backendtype", "Backend", "", 8, 2, backend.c_str());

    String model = "";
    switch (GlobalConfig.SonoffModel) {
      case SonoffModel_Switch:
        model = F("<option selected value='0'>Switch/Touch/S20</option><option value='1'>POW</option><option value='2'>Touch als Sender</option><option value='3'>Obi Zwischenstecker</option>");
        break;
      case SonoffModel_Pow:
        model = F("<option value='0'>Switch/Touch/S20</option><option selected value='1'>POW</option><option value='2'>Touch als Sender</option><option value='3'>Obi Zwischenstecker</option>");
        break;
      case SonoffModel_TouchAsSender:
        model = F("<option value='0'>Switch/Touch/S20</option><option value='1'>POW</option><option selected value='2'>Touch als Sender</option><option value='3'>Obi Zwischenstecker</option>");
        break;
      case SonoffModel_ObiZwischenstecker:
        model = F("<option value='0'>Switch/Touch/S20</option><option value='1'>POW</option><option value='2'>Touch als Sender</option><option selected value='3'>Obi Zwischenstecker</option>");
        break;
      default:
        model = F("<option selected value='0'>Switch/Touch/S20</option><option value='1'>POW</option><option value='2'>Touch als Sender</option><option value='3'>Obi Zwischenstecker</option>");
        break;
    }
    WiFiManagerParameter custom_sonoffmodel("sonoffmodel", "Sonoff Modell", "", 8, 2, model.c_str());

    String gpio14 = "";
    switch (GlobalConfig.GPIO14Mode) {
      case GPIO14Mode_OFF:
        gpio14 = F("<option selected value='0'>nicht verwendet</option><option value='1'>Taster</option><option value='2'>Schalter (absolut)</option><option value='3'>Schalter (toggle)</option>");
        break;
      case GPIO14Mode_KEY:
        gpio14 = F("<option value='0'>nicht verwendet</option><option selected value='1'>Taster</option><option value='2'>Schalter (absolut)</option><option value='3'>Schalter (toggle)</option>");
        break;
      case GPIO14Mode_SWITCH_ABSOLUT:
        gpio14 = F("<option value='0'>nicht verwendet</option><option value='1'>Taster</option><option selected value='2'>Schalter (absolut)</option><option value='3'>Schalter (toggle)</option>");
        break;
      case GPIO14Mode_SWITCH_TOGGLE:
        gpio14 = F("<option value='0'>nicht verwendet</option><option value='1'>Taster</option><option value='2'>Schalter (absolut)</option><option selected value='3'>Schalter (toggle)</option>");
        break;
      default:
        gpio14 = F("<option selected value='0'>nicht verwendet</option><option value='1'>Taster</option><option value='2'>Schalter</option>");
        break;
    }
    WiFiManagerParameter custom_gpio14mode("gpio14mode_switch", "GPIO14 Mode", "", 8, 2, gpio14.c_str());

    WiFiManagerParameter custom_gpio14assender("custom_gpio14assender_switch", "GPIO14 nur Sender: ", (GlobalConfig.GPIO14asSender) ? "1" : "0", 8, 1);

    WiFiManagerParameter custom_ip("custom_ip", "IP-Adresse", (String(SonoffNetConfig.ip) != "0.0.0.0") ? SonoffNetConfig.ip : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_netmask("custom_netmask", "Netzmaske", (String(SonoffNetConfig.netmask) != "0.0.0.0") ? SonoffNetConfig.netmask : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_gw("custom_gw", "Gateway",  (String(SonoffNetConfig.gw) != "0.0.0.0") ? SonoffNetConfig.gw : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_text("<br/><br><div>Statische IP (wenn leer, dann DHCP):</div>");
    wifiManager.addParameter(&custom_sonoffmodel);
    wifiManager.addParameter(&custom_ccuip);
    //wifiManager.addParameter(&custom_loxusername);
    //wifiManager.addParameter(&custom_loxpassword);
    wifiManager.addParameter(&custom_loxudpport);
    wifiManager.addParameter(&custom_sonoffname);
    wifiManager.addParameter(&custom_powervariablename);
    wifiManager.addParameter(&custom_ecvariablename);
    wifiManager.addParameter(&custom_powermeasureinterval);
    wifiManager.addParameter(&custom_restorestate);
    wifiManager.addParameter(&custom_cbleddisabled);
    wifiManager.addParameter(&custom_gpio14mode);
    wifiManager.addParameter(&custom_gpio14assender);
    wifiManager.addParameter(&custom_backendtype);
    wifiManager.addParameter(&custom_text);
    wifiManager.addParameter(&custom_ip);
    wifiManager.addParameter(&custom_netmask);
    wifiManager.addParameter(&custom_gw);

    wifiManager.setConfigPortalTimeout(ConfigPortalTimeout);


    if (startWifiManager == true) {
      if (_ssid == "" || _psk == "" ) {
        wifiManager.resetSettings();
      }
      else {
        if (!wifiManager.startConfigPortal()) {
          DEBUG(F("WM: failed to connect and hit timeout"));
          delay(500);
          ESP.restart();
        }
      }
    }

    wifiManager.setSTAStaticIPConfig(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]));

    wifiManager.autoConnect();

    DEBUG(F("Wifi Connected"));
    if (wm_shouldSaveConfig) {
      if (String(custom_ip.getValue()).length() > 5) {
        DEBUG("Custom IP Address is set!");
        strcpy(SonoffNetConfig.ip, custom_ip.getValue());
        strcpy(SonoffNetConfig.netmask, custom_netmask.getValue());
        strcpy(SonoffNetConfig.gw, custom_gw.getValue());

      } else {
        strcpy(SonoffNetConfig.ip,      "0.0.0.0");
        strcpy(SonoffNetConfig.netmask, "0.0.0.0");
        strcpy(SonoffNetConfig.gw,      "0.0.0.0");
      }

      GlobalConfig.restoreOldRelayState = (atoi(custom_restorestate.getValue()));
      GlobalConfig.LEDDisabled = (atoi(custom_cbleddisabled.getValue()) == 1);
      GlobalConfig.GPIO14asSender = (atoi(custom_gpio14assender.getValue()) == 1);
      GlobalConfig.BackendType = (atoi(custom_backendtype.getValue()));
      GlobalConfig.SonoffModel = (atoi(custom_sonoffmodel.getValue()));
      GlobalConfig.GPIO14Mode = (atoi(custom_gpio14mode.getValue()));

      strcpy(GlobalConfig.ccuIP, custom_ccuip.getValue());
      strcpy(GlobalConfig.DeviceName, custom_sonoffname.getValue());
      //strcpy(LoxoneConfig.Username, custom_loxusername.getValue());
      //strcpy(LoxoneConfig.Password, custom_loxpassword.getValue());
      strcpy(LoxoneConfig.UDPPort, custom_loxudpport.getValue());

      strcpy(HomeMaticConfig.PowerVariableName, custom_powervariablename.getValue());
      strcpy(HomeMaticConfig.EnergyCounterVariableName, custom_ecvariablename.getValue());

      GlobalConfig.MeasureInterval = atoi(custom_powermeasureinterval.getValue());

      saveSystemConfig();

      delay(100);
      ESP.restart();
    }
    DEBUG("CUSTOM STATIC IP: " + String(SonoffNetConfig.ip) + " Netmask: " + String(SonoffNetConfig.netmask) + " GW: " + String(SonoffNetConfig.gw));
    return true;
  }
}

void configModeCallback (WiFiManager *myWiFiManager) {
  DEBUG("AP-Modus ist aktiv!");
}

void saveConfigCallback () {
  DEBUG("Should save config");
  wm_shouldSaveConfig = true;
}

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);
    str = strchr(str, sep);
    if (str == NULL || *str == '\0') {
      break;
    }
    str++;
  }
}

void printWifiStatus() {
  DEBUG("SSID: " + WiFi.SSID());
  DEBUG("IP Address: " + IpAddress2String(WiFi.localIP()));
  DEBUG("Gateway Address: " + IpAddress2String(WiFi.gatewayIP()));
  DEBUG("signal strength (RSSI):" + String(WiFi.RSSI()) + " dBm");
}
