const char WM_HTTP_TITLE_LABEL[] PROGMEM = "<div class='l lt'><label>{v}</label><hr /></div>";
const char WM_HTTP_CURRENT_STATE_LABEL[] PROGMEM = "<div class='l ls'><label id='_ls'>{ls}</label></div>";
const char WM_HTTP_FW_LABEL[] PROGMEM = "<div class='l c k'><label>Firmware: {fw}</label></div>";
const char WM_HTTP_POWER_LABEL[] PROGMEM = "<table><tr><td class=tdl>Spannung</td><td class=tdr id='_v'>{hlw_v}</td><td class=tdl>V</td></tr><tr><td class=tdl>Strom</td><td class=tdr id='_c'>{hlw_c}</td><td class=tdl>A</td><tr><td class=tdl>Leistung</td><td class=tdr id='_w'>{hlw_w}</td><td class=tdl>W</td></tr><tr><td class=tdl>Leistung</td><td class=tdr id='_va'>{hlw_va}</td><td class=tdl>VA</td></tr><tr><td class=tdl>Energiez&auml;hler</td><td class=tdr id='_ec'>{hlw_ec}</td><td class=tdl>Wh</td></tr></table>";
const char WM_HTTP_ONOFF_BUTTONS[] PROGMEM = "<span class='l'><div><button name='btnAction' onclick='SetState(\"/1?ts=1&t=\"+document.getElementById(\"timer\").value); return false;'>AN</button></div><div><table><tr><td>Timer:</td><td align='right'><input class='i' type='text' id='timer' name='timer' placeholder='Sekunden' pattern='[0-9]{1,5}' value='' maxlength='5'></td></tr></table></div><div><button name='btnAction' onclick='SetState(\"/0?ts=1\"); return false;'>AUS</button></div></span>";
const char WM_HTTP_CONFIG_BUTTON[] PROGMEM = "<div></div><hr /><div></div><div><input class='lnkbtn' type='button' value='Konfiguration' onclick=\"window.location.href='/config'\" /></div>";
const char WM_HTTP_HOME_BUTTON[] PROGMEM = "<div><input class='lnkbtn' type='button' value='Zur&uuml;ck' onclick=\"window.location.href='/'\" /></div>";
const char WM_HTTP_SAVE_BUTTON[] PROGMEM = "<div><button name='btnSave' value='1' type='submit'>Speichern</button></div>";
const char WM_HTTP_CALIB_BUTTON[] PROGMEM = "<div></div><div><input class='lnkbtn' type='button' value='Kalibrieren' onclick=\"window.location.href='/calibrate'\" /></div>";
const char WM_HTTP_CALIB_INPUT[] PROGMEM = "<div><table><tr><td>Last (W):</td><td align='right'><input class='i' type='text' id='cwatt' name='cwatt' placeholder='Watt' pattern='[0-9]{1,4}'></td></tr><tr><td>Spannung (V):</td><td align='right'><input class='i' type='text' id='cvolt' name='cvolt' placeholder='Volt' pattern='[0-9]{1,3}' value='230'></td></tr></table></div>";
const char WM_HTTP_DOCALIB_BUTTON[] PROGMEM = "<div><button name='doCalibrate' value='1' type='submit'>Kalibrieren</button></div>";
const char WM_HTTP_UNDOCALIB_BUTTON[] PROGMEM = "<div><button name='undoCalibrate' value='1' type='submit'>Kalib. Reset</button></div>";
const char WM_HTTP_CONF[] PROGMEM = "<div><label>{st}:</label></div><div><input type='text' id='ccuip' name='ccuip' pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$' maxlength=16 placeholder='{st}' value='{ccuip}'></div><div><label>Ger&auml;tename:</label></div><div><input type='text' id='devicename' name='devicename' pattern='[A-Za-z0-9_ -]+' placeholder='Ger&auml;tename' value='{dn}'></div>";
const char WM_HTTP_CONF_ADD_RESTORESTATE[] PROGMEM = "<div><label for='restorestate'>Schaltzustand bei Boot</label><span class='ckb cob'><select id='restorestate' name='restorestate'><option {restore_off} value='0'>Aus</option><option {restore_last} value='1'>letzter Zustand</option><option {restore_on} value='2'>Ein</option></select></span></div>";
const char WM_HTTP_CONF_ADD_SWITCH[] PROGMEM = "<hr /><div><label>Nur Sonoff Switch:</label><div id='div_gpio14mode'></div><label for='gpio14mode'>GPIO14 Mode</label><span class='ckb cob'><select id='gpio14mode' name='gpio14mode'><option {gpio14mode_off} value='0'>nicht verwendet</option><option {gpio14mode_key} value='1'>Taster</option><option {gpio14mode_switch_abs} value='2'>Schalter (absolut)</option><option {gpio14mode_switch_tog} value='3'>Schalter (toggle)</option></select></span></div><div><label class='lcb' for='gpio14assender'><input id='gpio14assender' class='cb' type='checkbox' name='gpio14assender' {gpio14assender} value=1> GPIO14 nur Sender</label><hr /></div>";
const char WM_HTTP_CONF_ADD_ALL[] PROGMEM = "<div><label class='lcb' for='leddisabled'><input id='leddisabled' class='cb' type='checkbox' name='leddisabled' {le} value=1> LED deaktiviert</label></div>";
const char WM_HTTP_CONF_POW_LOAD_EC_ON_BOOT[] PROGMEM = "<div><label class='lcb' for='loadeconboot'><input id='loadeconboot' class='cb' type='checkbox' name='loadeconboot' {lecb} value=1> Lade Energiez&auml;hler von CCU beim Starten</label></div>";
const char WM_HTTP_CONF_POW_MEASURE_INTERVAL[] PROGMEM = "<div></div><div><label>Messintervall</label></div><div><input type='text' id='measureinterval' name='measureinterval' placeholder='Messintervall' pattern='[0-9]{2,3}' value='{mi}'></div>";
const char WM_HTTP_CONF_LOX[] PROGMEM = "<div><label>UDP Port:</label></div><div><input type='text' id='lox_udpport' pattern='[0-9]{1,5}' maxlength='5' name='lox_udpport' placeholder='UDP Port' value='{udp}'></div>";
const char WM_HTTP_CONF_HM_POW[] PROGMEM  = "<div><label>Variable f&uuml;r Leistungswert:</label></div><div><input type='text' id='hmpowvar' name='hmpowvar' placeholder='Variablenname' value='{hmpowvar}' pattern='[A-Za-z0-9_ -]+'></div><div><label>Variable f&uuml;r Energiez&auml;hler:</label></div><div><input type='text' id='hmecvar' name='hmecvar' placeholder='Variablenname' value='{hmecvar}' pattern='[A-Za-z0-9_ -]+'></div>";
const char WM_HTTP_STATUSLABEL[] PROGMEM = "<div class='l c'>{sl}</div>";
const char WM_HTTP_NEWFW_BUTTON[] PROGMEM = "<div><input class='fwbtn' id='fwbtn' type='button' value='Neue Firmware verf&uuml;gbar' onclick=\"window.open('{fwurl}')\" /></div><div><input class='fwbtn' id='fwbtnupdt' type='button' value='Firmwaredatei einspielen' onclick=\"window.location.href='/update'\" /></div>";

void initWebServerHandler() {
  WebServer.on("/0", webSwitchRelayOff);
  WebServer.on("/off", webSwitchRelayOff);
  WebServer.on("/1", webSwitchRelayOn);
  WebServer.on("/on", webSwitchRelayOn);
  WebServer.on("/2", webToggleRelay);
  WebServer.on("/toggle", webToggleRelay);
  WebServer.on("/addEnergyCounter", addEnergyCounter);
  WebServer.on("/enableEnergyCounterTransmission", enableEnergyCounterTransmission);
  WebServer.on("/resetEnergyCounter", resetEnergyCounter);
  WebServer.on("/getState", replyRelayState);
  WebServer.on("/bootConfigMode", setBootConfigMode);
  WebServer.on("/reboot", []() {
    WebServer.send(200, "text/plain", "rebooting");
    delay(100);
    ESP.restart();
  });
  WebServer.on("/restart", []() {
    WebServer.send(200, "text/plain", "rebooting");
    delay(100);
    ESP.restart();
  });
  WebServer.on("/version", versionHtml);
  WebServer.on("/firmware", versionHtml);
  WebServer.on("/config", configHtml);
  WebServer.on("/calibrate", calibrateHtml);
  WebServer.on("/getPower", replyPower);
  WebServer.on("/getPowerJSON", replyPowerJSON);
  WebServer.on("/reloadCUxD", []() {
    String ret = reloadCUxDAddress(TRANSMITSTATE);
    WebServer.send(200, "text/plain", ret);
  });
  WebServer.onNotFound(defaultHtml);
}

void addEnergyCounter() {
  if (GlobalConfig.SonoffModel != SonoffModel_Pow) {
    WebServer.send(200, "text/plain", "Only for Sonoff POW");
  } else {
    if (WebServer.args() > 0) {
      for (int i = 0; i < WebServer.args(); i++) {
        if (WebServer.argName(i) == "value") {
          String argValue = WebServer.arg(i);
          argValue.replace(",", ".");
          float prev_energy_counter = hlw8012value.energy_counter;
          hlw8012value.energy_counter = hlw8012value.energy_counter + argValue.toFloat();
          WebServer.send(200, "text/plain", "Old Energy Counter: " + String(prev_energy_counter) + "; Added Value: " + String(argValue.toFloat()) + "; New Energy Counter: " + String(hlw8012value.energy_counter));
        }
      }
    }
  }
}

void enableEnergyCounterTransmission() {
  if (GlobalConfig.SonoffModel != SonoffModel_Pow) {
    WebServer.send(200, "text/plain", "Only for Sonoff POW");
  } else {
    if (!HomeMaticConfig.EnergyCounterVariableAvailable) {
      if (WebServer.args() > 0) {
        for (int i = 0; i < WebServer.args(); i++) {
          if (WebServer.argName(i) == "load") {
            bool setValue = WebServer.arg(i).toInt();
            if (getEnergyCounterValueFromCCU(setValue)) {
              WebServer.send(200, "text/plain", "Energy Counter enabled");
            } else {
              WebServer.send(200, "text/plain", "Energy Counter NOT enabled");
            }
          }
        }
      } else {
        WebServer.send(500, "text/plain", "Missing argument");
      }
    } else {
      WebServer.send(200, "text/plain", "Energy Counter is already enabled");
    }
  }
}

void resetEnergyCounter() {
  if (GlobalConfig.SonoffModel != SonoffModel_Pow) {
    WebServer.send(200, "text/plain", "Only for Sonoff POW");
  } else {
    hlw8012value.energy_counter = 0.0;
    WebServer.send(200, "text/plain", "Energy Counter Reset OK");
  }
}
void webSwitchRelayOn() {
  bool _transmitstate = NO_TRANSMITSTATE;
  if (WebServer.args() > 0) {
    for (int i = 0; i < WebServer.args(); i++) {
      if (WebServer.argName(i) == "t") {
        TimerSeconds = WebServer.arg(i).toInt();
        if (TimerSeconds > 0) {
          TimerStartMillis = millis();
          DEBUG("webSwitchRelayOn(), Timer aktiviert, Sekunden: " + String(TimerSeconds));
        } else {
          DEBUG(F("webSwitchRelayOn(), Parameter, aber mit TimerSeconds = 0"));
        }
      }
      if (WebServer.argName(i) == "ts") {
        _transmitstate = WebServer.arg(i).toInt();
      }
    }
  } else {
    TimerSeconds = 0;
    DEBUG(F("webSwitchRelayOn(), keine Parameter, TimerSeconds = 0"));
  }
  switchRelay(RELAYSTATE_ON, _transmitstate);
  sendDefaultWebCmdReply();
}

void webToggleRelay() {
  bool _transmitstate = NO_TRANSMITSTATE;
  if (WebServer.args() > 0) {
    for (int i = 0; i < WebServer.args(); i++) {
      if (WebServer.argName(i) == "ts") {
        _transmitstate = WebServer.arg(i).toInt();
      }
    }
  }
  toggleRelay(_transmitstate);
  sendDefaultWebCmdReply();
}
void webSwitchRelayOff() {
  bool _transmitstate = NO_TRANSMITSTATE;
  if (WebServer.args() > 0) {
    for (int i = 0; i < WebServer.args(); i++) {
      if (WebServer.argName(i) == "ts") {
        _transmitstate = WebServer.arg(i).toInt();
      }
    }
  }
  switchRelay(RELAYSTATE_OFF, _transmitstate);
  sendDefaultWebCmdReply();
}

void replyRelayState() {
  sendDefaultWebCmdReply();
}

void calibrateHtml() {
  if (GlobalConfig.SonoffModel != SonoffModel_Pow) {
    WebServer.send(200, "text/plain", "Only for Sonoff POW");
  } else {
    bool doCalibrate = false;
    bool undoCalibrate = false;
    byte cvolt = 0;
    int cwatt = 0;
    if (WebServer.args() > 0) {
      for (int i = 0; i < WebServer.args(); i++) {
        if (WebServer.argName(i) == "undoCalibrate") {
          undoCalibrate = (WebServer.arg(i).toInt() == 1);
        }
        if (WebServer.argName(i) == "doCalibrate") {
          doCalibrate = (WebServer.arg(i).toInt() == 1);
        }
        if (WebServer.argName(i) == "cwatt") {
          cwatt = (WebServer.arg(i).toInt());
        }
        if (WebServer.argName(i) == "cvolt") {
          cvolt = (WebServer.arg(i).toInt());
        }
      }
    }

    String page = FPSTR(WM_HTTP_HEAD);
    //page += FPSTR(WM_HTTP_SCRIPT);
    page += FPSTR(WM_HTTP_ALL_STYLE);
    if (GlobalConfig.BackendType == BackendType_HomeMatic)
      page += FPSTR(WM_HTTP_HM_STYLE);
    if (GlobalConfig.BackendType == BackendType_Loxone)
      page += FPSTR(WM_HTTP_LOX_STYLE);
    page += FPSTR(WM_HTTP_HEAD_END);
    page += F("<div class='fbg'>");
    page += F("<form method='post' action='calibrate'>");
    page += FPSTR(WM_HTTP_TITLE_LABEL);
    page += FPSTR(WM_HTTP_CALIB_INPUT);

    if (undoCalibrate) {
      page += FPSTR(WM_HTTP_STATUSLABEL);
      page.replace("{sl}", F("benutzerdef. Kalibrierungsdaten wurden gel&ouml;scht!"));
      hlwundocalibrate();
    }

    if (doCalibrate && cvolt > 0 && cwatt > 0 && hlw8012value.voltage > 0) {
      page += FPSTR(WM_HTTP_STATUSLABEL);
      page.replace("{sl}", F("Kalibrierung gestartet!<br>Achtung:<br>Das Ger&auml;t reagiert nun ca. 10 Sekunden lang nicht!"));
    }

    page += FPSTR(WM_HTTP_DOCALIB_BUTTON);
    page += FPSTR(WM_HTTP_UNDOCALIB_BUTTON);
    page += FPSTR(WM_HTTP_HOME_BUTTON);

    page += F("</form></div>");
    page += F("</body></html>");
    page.replace("{v}", GlobalConfig.DeviceName);
    WebServer.sendHeader("Content-Length", String(page.length()));
    WebServer.send(200, "text/html", page);

    if (doCalibrate && cvolt > 0 && cwatt > 0 && hlw8012value.voltage > 0) {
      DEBUG("Starte Kalibrierung");
      hlwcalibrate(cvolt, cwatt);
    }
  }
}

void defaultHtml() {
  if (WebServer.args() > 0) {
    for (int i = 0; i < WebServer.args(); i++) {
      if (WebServer.argName(i) == "btnAction") {
        switchRelay(WebServer.arg(i).toInt(), TRANSMITSTATE);
      }
      if (WebServer.argName(i) == "timer") {
        TimerSeconds = WebServer.arg(i).toInt();
        if (TimerSeconds > 0) {
          TimerStartMillis = millis();
        }
      }
    }
  }

  String page = FPSTR(WM_HTTP_HEAD);
  //page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_ALL_STYLE);
  if (GlobalConfig.BackendType == BackendType_HomeMatic)
    page += FPSTR(WM_HTTP_HM_STYLE);
  if (GlobalConfig.BackendType == BackendType_Loxone)
    page += FPSTR(WM_HTTP_LOX_STYLE);
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<div class='fbg'>");

  //page += F("<form method='post' action='control'>");
  page += FPSTR(WM_HTTP_TITLE_LABEL);
  page += FPSTR(WM_HTTP_CURRENT_STATE_LABEL);
  page.replace("{v}", GlobalConfig.DeviceName);

  page.replace("{ls}", ((getRelayState() == RELAYSTATE_ON) ? "AN" : "AUS"));

  page += FPSTR(WM_HTTP_ONOFF_BUTTONS);

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(WM_HTTP_POWER_LABEL);
    page.replace("{hlw_c}", String(hlw8012value.current));
    page.replace("{hlw_v}", String(hlw8012value.voltage));
    page.replace("{hlw_w}", String(hlw8012value.powerw));
    page.replace("{hlw_va}", String(hlw8012value.powerva));
    page.replace("{hlw_ec}", String(hlw8012value.energy_counter));
  }

  page += FPSTR(WM_HTTP_CONFIG_BUTTON);
  String restZeit = "";
  if (TimerSeconds > 0) restZeit =  String(TimerSeconds - (millis() - TimerStartMillis) / 1000) ;
  page.replace("{ts}", restZeit);

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(WM_HTTP_CALIB_BUTTON);
  }

  page += FPSTR(WM_HTTP_FW_LABEL);

  page += FPSTR(WM_HTTP_NEWFW_BUTTON);
  String fwurl = FPSTR(GITHUB_REPO_URL);
  String fwjsurl = FPSTR(GITHUB_REPO_URL);
  fwurl.replace("api.", "");
  fwurl.replace("repos/", "");
  page.replace("{fwurl}", fwurl);

  page += F("</div><script>");
  page += FPSTR(WM_HTTP_CUSTOMSCRIPT);
  page += FPSTR(WM_HTTP_CUSTOMUPDATESCRIPT);
  page.replace("{fwjsurl}", fwjsurl);
  page.replace("{fw}", FIRMWARE_VERSION);
  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(WM_HTTP_CUSTOMPOWSCRIPT);
    page.replace("{lecb}", ((GlobalConfig.loadEcOnBoot) ? "checked" : ""));
    page.replace("{mi}", String(GlobalConfig.MeasureInterval * 1000));
  }

  page += F("</script></div></body></html>");
  WebServer.sendHeader("Content-Length", String(page.length()));
  WebServer.send(200, "text/html", page);
}

void configHtml() {
  bool sc = false;
  bool saveSuccess = false;
  bool showHMDevError = false;
  if (WebServer.args() > 0) {
    GlobalConfig.restoreOldRelayState = RelayStateOnBoot_OFF;
    GlobalConfig.loadEcOnBoot = false;
    GlobalConfig.LEDDisabled = false;
    GlobalConfig.GPIO14asSender = false;
    for (int i = 0; i < WebServer.args(); i++) {
      if (WebServer.argName(i) == "btnSave")
        sc = (WebServer.arg(i).toInt() == 1);
      if (WebServer.argName(i) == "ccuip")
        strcpy(GlobalConfig.ccuIP, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "measureinterval")
        GlobalConfig.MeasureInterval = String(WebServer.arg(i)).toInt();
      if  (WebServer.argName(i) == "hmpowvar")
        strcpy(HomeMaticConfig.PowerVariableName, WebServer.arg(i).c_str());
      if  (WebServer.argName(i) == "hmecvar")
        strcpy(HomeMaticConfig.EnergyCounterVariableName, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "devicename")
        strcpy(GlobalConfig.DeviceName, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "lox_udpport")
        strcpy(LoxoneConfig.UDPPort, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "restorestate")
        GlobalConfig.restoreOldRelayState = String(WebServer.arg(i)).toInt();
      if (WebServer.argName(i) == "leddisabled")
        GlobalConfig.LEDDisabled = (String(WebServer.arg(i)).toInt() == 1);
      if (WebServer.argName(i) == "loadeconboot")
        GlobalConfig.loadEcOnBoot = (String(WebServer.arg(i)).toInt() == 1);
      if (WebServer.argName(i) == "gpio14mode")
        GlobalConfig.GPIO14Mode = String(WebServer.arg(i)).toInt();
      if (WebServer.argName(i) == "gpio14assender")
        GlobalConfig.GPIO14asSender = (String(WebServer.arg(i)).toInt() == 1);
    }
    if (sc) {
      setLastRelayState(getRelayState() == RELAYSTATE_ON);
      saveSuccess = saveSystemConfig();
      if (GlobalConfig.BackendType == BackendType_HomeMatic) {
        String devName = getStateCUxD(String(GlobalConfig.DeviceName), "Address") ;
        if (devName != "null") {
          showHMDevError = false;
          HomeMaticConfig.ChannelName =  "CUxD." + devName;
        } else {
          showHMDevError = true;
        }
        switchLED((RelayState ? On : Off));
      }
    }
  }
  String page = FPSTR(WM_HTTP_HEAD);

  //page += FPSTR(WM_HTTP_SCRIPT);
  page += FPSTR(WM_HTTP_ALL_STYLE);
  if (GlobalConfig.BackendType == BackendType_HomeMatic)
    page += FPSTR(WM_HTTP_HM_STYLE);
  if (GlobalConfig.BackendType == BackendType_Loxone)
    page += FPSTR(WM_HTTP_LOX_STYLE);
  page += FPSTR(WM_HTTP_HEAD_END);
  page += F("<div class='fbg'>");
  page += F("<form method='post' action='config'>");
  page += FPSTR(WM_HTTP_TITLE_LABEL);
  page += FPSTR(WM_HTTP_CONF);

  page += FPSTR(WM_HTTP_CONF_ADD_RESTORESTATE);
  switch (GlobalConfig.restoreOldRelayState) {
    case RelayStateOnBoot_OFF:
      page.replace("{restore_off}", "selected");
      page.replace("{restore_last}", "");
      page.replace("{restore_on}", "");
      break;
    case RelayStateOnBoot_LAST:
      page.replace("{restore_off}", "");
      page.replace("{restore_last}", "selected");
      page.replace("{restore_on}", "");
      break;
    case RelayStateOnBoot_ON:
      page.replace("{restore_off}", "");
      page.replace("{restore_last}", "");
      page.replace("{restore_on}", "selected");
      break;
  }

  if (GlobalConfig.SonoffModel == SonoffModel_Switch) {
    page += FPSTR(WM_HTTP_CONF_ADD_SWITCH);
    switch (GlobalConfig.GPIO14Mode) {
      case GPIO14Mode_OFF:
        page.replace("{gpio14mode_off}", "selected");
        page.replace("{gpio14mode_key}", "");
        page.replace("{gpio14mode_switch_abs}", "");
        page.replace("{gpio14mode_switch_tog}", "");
        break;
      case GPIO14Mode_KEY:
        page.replace("{gpio14mode_off}", "");
        page.replace("{gpio14mode_key}", "selected");
        page.replace("{gpio14mode_switch_abs}", "");
        page.replace("{gpio14mode_switch_tog}", "");
        break;
      case GPIO14Mode_SWITCH_ABSOLUT:
        page.replace("{gpio14mode_off}", "");
        page.replace("{gpio14mode_key}", "");
        page.replace("{gpio14mode_switch_abs}", "selected");
        page.replace("{gpio14mode_switch_tog}", "");
        break;
      case GPIO14Mode_SWITCH_TOGGLE:
        page.replace("{gpio14mode_off}", "");
        page.replace("{gpio14mode_key}", "");
        page.replace("{gpio14mode_switch_abs}", "");
        page.replace("{gpio14mode_switch_tog}", "selected");
        break;
      default:
        page.replace("{gpio14mode_off}", "selected");
        page.replace("{gpio14mode_key}", "");
        page.replace("{gpio14mode_switch_abs}", "");
        page.replace("{gpio14mode_switch_tog}", "");
        break;
    }
    page.replace("{gpio14assender}", ((GlobalConfig.GPIO14asSender) ? "checked" : ""));
  }

  page += FPSTR(WM_HTTP_CONF_ADD_ALL);

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(WM_HTTP_CONF_POW_LOAD_EC_ON_BOOT);
    page += FPSTR(WM_HTTP_CONF_POW_MEASURE_INTERVAL);
  }
  if (GlobalConfig.BackendType == BackendType_HomeMatic) {
    if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
      page += FPSTR(WM_HTTP_CONF_HM_POW);
      page.replace("{hmpowvar}", HomeMaticConfig.PowerVariableName);
      page.replace("{hmecvar}", HomeMaticConfig.EnergyCounterVariableName);
    }
    page.replace("{st}", "CCU2 IP");
    page.replace("{remanenz}", "Schaltzustand wiederherstellen");
  }
  if (GlobalConfig.BackendType == BackendType_Loxone) {
    page += FPSTR(WM_HTTP_CONF_LOX);
    page.replace("{st}", "MiniServer IP");
    page.replace("{udp}", LoxoneConfig.UDPPort);
    page.replace("{remanenz}", "Remanenzeingang");
  }

  page.replace("{le}", ((GlobalConfig.LEDDisabled) ? "checked" : ""));
  page.replace("{lecb}", ((GlobalConfig.loadEcOnBoot) ? "checked" : ""));
  page.replace("{dn}", GlobalConfig.DeviceName);
  page.replace("{ccuip}", GlobalConfig.ccuIP);
  page.replace("{mi}", String(GlobalConfig.MeasureInterval));

  page += FPSTR(WM_HTTP_STATUSLABEL);

  if (sc && !showHMDevError) {
    if (saveSuccess) {
      page.replace("{sl}", F("<label class='green'>Speichern erfolgreich.</label>"));
    } else {
      page.replace("{sl}", F("<label class='red'>Speichern fehlgeschlagen.</label>"));
    }
  }

  if (showHMDevError)
    page.replace("{sl}", F("<label class='red'>Ger&auml;tenamen in CUxD pr&uuml;fen!</label>"));

  if (!sc && !showHMDevError)
    page.replace("{sl}", "");

  page += FPSTR(WM_HTTP_SAVE_BUTTON);
  page += FPSTR(WM_HTTP_HOME_BUTTON);
  page += FPSTR(WM_HTTP_FW_LABEL);
  page.replace("{fw}", FIRMWARE_VERSION);

  page += F("</form></div>");
  page += F("</div></body></html>");
  page.replace("{v}", GlobalConfig.DeviceName);

  WebServer.send(200, "text/html", page);
}

void sendDefaultWebCmdReply() {
  String reply = createReplyString();
  DEBUG("Sending Web-Reply: " + reply);
  WebServer.send(200, "application/json", reply);
}

String createReplyString() {
  return "{\"state\": " + String(getRelayState() == RELAYSTATE_ON) + ", \"timer\": " + String(TimerSeconds) + ", \"resttimer\": " + String((TimerSeconds > 0) ? (TimerSeconds - (millis() - TimerStartMillis) / 1000) : 0) + ", \"fw\": \"" + FIRMWARE_VERSION + "\"}";
}

void versionHtml() {
  WebServer.send(200, "text/plain", "<fw>" + FIRMWARE_VERSION + "</fw>");
}

void replyPowerJSON() {
  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    WebServer.send(200, "application/json", "{\"Voltage\":\"" + String(hlw8012value.voltage) + "\",\"Current\":\"" + String(hlw8012value.current) + "\",\"PowerW\":\"" + String(hlw8012value.powerw) + "\",\"PowerVA\":\"" + String(hlw8012value.powerva) + "\",\"EnergyCounter\":\"" + String(hlw8012value.energy_counter) + "\"}");
  else
    WebServer.send(200, "application/json", "{\"Voltage\":\"NaN\",\"Current\":\"NaN\",\"PowerW\":\"NaN\",\"PowerVA\":\"NaN\",\"EnergyCounter\":\"NaN\"}");
}

void replyPower() {
  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    WebServer.send(200, "text/plain", "<voltage>" + String(hlw8012value.voltage) + "</voltage><current>" + String(hlw8012value.current) + "</current><powerw>" + String(hlw8012value.powerw) + "</powerw><powerva>" + String(hlw8012value.powerva) + "</powerva><energycounter>" + String(hlw8012value.energy_counter) + "</energycounter>");
  else
    WebServer.send(200, "text/plain", "not supported");
}
