const char HTTP_TITLE_LABEL[] PROGMEM = "<div class='l lt'><label>{v}</label><hr /></div>";
const char HTTP_CURRENT_STATE_LABEL[] PROGMEM = "<div class='l ls'><label id='_ls'>{ls}</label></div>";
const char HTTP_FW_LABEL[] PROGMEM = "<div class='l c k'><label>Firmware: {fw}</label></div>";
const char HTTP_POWER_LABEL[] PROGMEM = "<table><tr><td class=tdl>Spannung</td><td class=tdr id='_v'>{hlw_v}</td><td class=tdl>V</td></tr><tr><td class=tdl>Strom</td><td class=tdr id='_c'>{hlw_c}</td><td class=tdl>A</td><tr><td class=tdl>Leistung</td><td class=tdr id='_w'>{hlw_w}</td><td class=tdl>W</td></tr><tr><td class=tdl>Leistung</td><td class=tdr id='_va'>{hlw_va}</td><td class=tdl>VA</td></tr></table>";
const char HTTP_ONOFF_BUTTONS[] PROGMEM = "<span class='l'><div><button name='btnAction' onclick='SetState(\"/1?t=\"+document.getElementById(\"timer\").value); return false;'>AN</button></div><div><table><tr><td>Timer:</td><td align='right'><input class='i' type='text' id='timer' name='timer' placeholder='Sekunden' pattern='[0-9]{1,5}' value='' maxlength='5'></td></tr></table></div><div><button name='btnAction' onclick='SetState(\"/0\"); return false;'>AUS</button></div></span>";
const char HTTP_CONFIG_BUTTON[] PROGMEM = "<div></div><hr /><div></div><div><input class='lnkbtn' type='button' value='Konfiguration' onclick=\"window.location.href='/config'\" /></div>";
const char HTTP_ALL_STYLE[] PROGMEM = "<style>div {white-space: nowrap;} input[type=text] {width:95%;} .green {color:green;} .red {color:red;} .tdr {float:right;} .tdl { width: 1px;} input.lnkbtn,input.fwbtn {-webkit-appearance: button;-moz-appearance: button;appearance: button;} body {background-color: #303030;} input.lnkbtn,button,input.fwbtn{cursor: pointer;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;padding:5px;} input,button,input.lnkbtn,input.fwbtn {border: 0;border-radius: 0.3rem;} .c{text-align: center;} .k{font-style:italic;} .fbg {background-color: #eee;} div,input{padding:5px;font-size:1em;} .i{text-align: right; width: 45% !important;} body{text-align: center;font-family:verdana;} .l{no-repeat left center;background-size: 1em;} .q{float: right;width: 64px;text-align: right;} .ls {font-weight: bold;text-align: center;font-size: 300%;} .lt{font-size: 150%;text-align: center;} table{width:100%;} td{max-width:50%;font-weight: bold;} input.fwbtn {display: none; background-color: #ff0000;} ";
const char HTTP_HM_STYLE[]  PROGMEM = "input.lnkbtn,button{background-color:#1fa3ec;}</style>";
const char HTTP_LOX_STYLE[] PROGMEM = "input.lnkbtn,button{background-color:#83b817;}</style>";
const char HTTP_HOME_BUTTON[] PROGMEM = "<div><input class='lnkbtn' type='button' value='Zur&uuml;ck' onclick=\"window.location.href='/'\" /></div>";
const char HTTP_SAVE_BUTTON[] PROGMEM = "<div><button name='btnSave' value='1' type='submit'>Speichern</button></div>";
const char HTTP_CALIB_BUTTON[] PROGMEM = "<div></div><div><input class='lnkbtn' type='button' value='Kalibrieren' onclick=\"window.location.href='/calibrate'\" /></div>";
const char HTTP_CALIB_INPUT[] PROGMEM = "<div><table><tr><td>Last (W):</td><td align='right'><input class='i' type='text' id='cwatt' name='cwatt' placeholder='Watt' pattern='[0-9]{1,4}'></td></tr><tr><td>Spannung (V):</td><td align='right'><input class='i' type='text' id='cvolt' name='cvolt' placeholder='Volt' pattern='[0-9]{1,3}' value='230'></td></tr></table></div>";
const char HTTP_DOCALIB_BUTTON[] PROGMEM = "<div><button name='doCalibrate' value='1' type='submit'>Kalibrieren</button></div>";
const char HTTP_UNDOCALIB_BUTTON[] PROGMEM = "<div><button name='undoCalibrate' value='1' type='submit'>Kalib. Reset</button></div>";
const char HTTP_CONF[] PROGMEM = "<div><label>{st}:</label></div><div><input type='text' id='ccuip' name='ccuip' pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$' maxlength=16 placeholder='{st}' value='{ccuip}'></div><div><label>Ger&auml;tename:</label></div><div><input type='text' id='devicename' name='devicename' pattern='[A-Za-z0-9_ -]+' placeholder='Ger&auml;tename' value='{dn}'></div><div><label for='rstate' class='lcb' title='Stellt den Schaltzustand nach einer Stromunterbrechung wiederher'><input class 'cb' id='rstate' type='checkbox' name='rstate' {rs} value=1> {remanenz}</label></div>";
const char HTTP_CONF_ADD_SWITCH[] PROGMEM = "<div><label class='lcb' for='leddisabled'><input id='leddisabled' class='cb' type='checkbox' name='leddisabled' {le} value=1> LED deaktiviert</label></div>";
const char HTTP_CONF_POW_MEASURE_INTERVAL[] PROGMEM = "<div></div><div><label>Messintervall</label></div><div><input type='text' id='measureinterval' name='measureinterval' placeholder='Messintervall' pattern='[0-9]{2,3}' value='{mi}'></div>";
const char HTTP_CONF_LOX[] PROGMEM = "<div><label>UDP Port:</label></div><div><input type='text' id='lox_udpport' pattern='[0-9]{1,5}' maxlength='5' name='lox_udpport' placeholder='UDP Port' value='{udp}'></div>";
const char HTTP_CONF_HM_POW[] PROGMEM  = "<div><label>Variable f&uuml;r Leistungswert:</label></div><div><input type='text' id='hmpowvar' name='hmpowvar' placeholder='Variablenname' value='{hmpowvar}' pattern='[A-Za-z0-9_ -]+'></div>";
const char HTTP_STATUSLABEL[] PROGMEM = "<div class='l c'>{sl}</div>";
const char HTTP_NEWFW_BUTTON[] PROGMEM = "<div><input class='fwbtn' id='fwbtn' type='button' value='Neue Firmware verf&uuml;gbar' onclick=\"window.open('{fwurl}')\" /></div>";
const char HTTP_CUSTOMSCRIPT[] PROGMEM = "var timerRun = false; function Get(u){ var h = new XMLHttpRequest(); h.open('GET',u,false); h.send(null); return h.responseText; } function SetState(v) { document.getElementById('timer').value = ''; var json_obj = JSON.parse(Get(v)); refreshState(json_obj, false); } function isInt(v){return !isNaN(v) && parseInt(Number(v))==v && !isNaN(parseInt(v,10));} function timerdecrement(t,rekursiv) { if (rekursiv) timerRun=false; if (isInt(t.placeholder)) {t.placeholder = t.placeholder-1; if (t.placeholder > 0 && !timerRun) timerRun = setTimeout(function(){ timerdecrement(t, true) }, 1000); else setTimeout(function(){ refreshState(null, false);}, 1000);  }} function refreshState(json_obj, rekursiv) { if (json_obj == null) json_obj = JSON.parse(Get('/getState')); document.getElementById('_ls').innerHTML = (('1' == json_obj.state) ? 'AN' : 'AUS'); var timer = document.getElementById('timer'); timer.placeholder = json_obj.resttimer; if (timer.placeholder == 0) timer.placeholder = 'Sekunden'; else timerdecrement(timer); if (rekursiv) setTimeout(function(){ refreshState(null, true); }, 10000); } /*init refresh:*/ refreshState(null, true); ";
const char HTTP_CUSTOMUPDATESCRIPT[] PROGMEM = "function updateCheck() { var json_obj = JSON.parse(Get('{fwjsurl}')); if (json_obj.tag_name != '{fw}'){document.getElementById('fwbtn').style.display='block';document.getElementById('fwbtn').title = json_obj.tag_name; } }; setTimeout(function(){ updateCheck() }, 3000); ";
const char HTTP_CUSTOMPOWSCRIPT[] PROGMEM = "setTimeout(function(){ refresh(); }, {mi}); function refresh() { var json_obj = JSON.parse(Get('/getPowerJSON')); document.getElementById('_v').innerHTML = json_obj.Voltage; document.getElementById('_c').innerHTML = json_obj.Current; document.getElementById('_w').innerHTML = json_obj.PowerW; document.getElementById('_va').innerHTML = json_obj.PowerVA; setTimeout(function(){ refresh() }, {mi}); } ";

void webSwitchRelayOn() {
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
    }
  } else {
    TimerSeconds = 0;
    DEBUG(F("webSwitchRelayOn(), keine Parameter, TimerSeconds = 0"));
  }
  switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
  sendDefaultWebCmdReply();
}

void webToggleRelay() {
  toggleRelay(NO_TRANSMITSTATE);
  sendDefaultWebCmdReply();
}
void webSwitchRelayOff() {
  switchRelay(RELAYSTATE_OFF, NO_TRANSMITSTATE);
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

    String page = FPSTR(HTTP_HEAD);
    //page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_ALL_STYLE);
    if (GlobalConfig.BackendType == BackendType_HomeMatic)
      page += FPSTR(HTTP_HM_STYLE);
    if (GlobalConfig.BackendType == BackendType_Loxone)
      page += FPSTR(HTTP_LOX_STYLE);
    page += FPSTR(HTTP_HEAD_END);
    page += F("<div class='fbg'>");
    page += F("<form method='post' action='calibrate'>");
    page += FPSTR(HTTP_TITLE_LABEL);
    page += FPSTR(HTTP_CALIB_INPUT);

    if (undoCalibrate) {
      page += FPSTR(HTTP_STATUSLABEL);
      page.replace("{sl}", F("benutzerdef. Kalibrierungsdaten wurden gel&ouml;scht!"));
      hlwundocalibrate();
    }

    if (doCalibrate && cvolt > 0 && cwatt > 0 && hlw8012value.voltage > 0) {
      page += FPSTR(HTTP_STATUSLABEL);
      page.replace("{sl}", F("Kalibrierung gestartet!<br>Achtung:<br>Das Ger&auml;t reagiert nun ca. 10 Sekunden lang nicht!"));
    }

    page += FPSTR(HTTP_DOCALIB_BUTTON);
    page += FPSTR(HTTP_UNDOCALIB_BUTTON);
    page += FPSTR(HTTP_HOME_BUTTON);

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
      if (WebServer.argName(i) == "btnAction")
        switchRelay(WebServer.arg(i).toInt(), TRANSMITSTATE);
      if (WebServer.argName(i) == "timer") {
        TimerSeconds = WebServer.arg(i).toInt();
        if (TimerSeconds > 0) {
          TimerStartMillis = millis();
        }
      }
    }
  }

  String page = FPSTR(HTTP_HEAD);
  //page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_ALL_STYLE);
  if (GlobalConfig.BackendType == BackendType_HomeMatic)
    page += FPSTR(HTTP_HM_STYLE);
  if (GlobalConfig.BackendType == BackendType_Loxone)
    page += FPSTR(HTTP_LOX_STYLE);
  page += FPSTR(HTTP_HEAD_END);
  page += F("<div class='fbg'>");

  //page += F("<form method='post' action='control'>");
  page += FPSTR(HTTP_TITLE_LABEL);
  page += FPSTR(HTTP_CURRENT_STATE_LABEL);
  page.replace("{v}", GlobalConfig.DeviceName);

  page.replace("{ls}", ((digitalRead(RelayPin) == HIGH) ? "AN" : "AUS"));

  page += FPSTR(HTTP_ONOFF_BUTTONS);

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(HTTP_POWER_LABEL);
    page.replace("{hlw_c}", String(hlw8012value.current));
    page.replace("{hlw_v}", String(hlw8012value.voltage));
    page.replace("{hlw_w}", String(hlw8012value.powerw));
    page.replace("{hlw_va}", String(hlw8012value.powerva));
  }

  page += FPSTR(HTTP_CONFIG_BUTTON);
  String restZeit = "";
  if (TimerSeconds > 0) restZeit =  String(TimerSeconds - (millis() - TimerStartMillis) / 1000) ;
  page.replace("{ts}", restZeit);

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(HTTP_CALIB_BUTTON);
  }

  page += FPSTR(HTTP_FW_LABEL);

  //  if (newFirmwareAvailable) {
  page += FPSTR(HTTP_NEWFW_BUTTON);
  String fwurl = FPSTR(GITHUB_REPO_URL);
  String fwjsurl = FPSTR(GITHUB_REPO_URL);
  fwurl.replace("api.", "");
  fwurl.replace("repos/", "");
  page.replace("{fwurl}", fwurl);
  //  }

  //page += F("</form></div><script>");
  page += F("</div><script>");
  page += FPSTR(HTTP_CUSTOMSCRIPT);
  page += FPSTR(HTTP_CUSTOMUPDATESCRIPT);
  page.replace("{fwjsurl}", fwjsurl);
  page.replace("{fw}", FIRMWARE_VERSION);
  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(HTTP_CUSTOMPOWSCRIPT);
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
    GlobalConfig.restoreOldRelayState = false;
    GlobalConfig.LEDDisabled = false;
    for (int i = 0; i < WebServer.args(); i++) {
      if (WebServer.argName(i) == "btnSave")
        sc = (WebServer.arg(i).toInt() == 1);
      if (WebServer.argName(i) == "ccuip")
        strcpy(GlobalConfig.ccuIP, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "measureinterval")
        GlobalConfig.MeasureInterval = String(WebServer.arg(i)).toInt();
      if  (WebServer.argName(i) == "hmpowvar")
        strcpy(HomeMaticConfig.PowerVariableName, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "devicename")
        strcpy(GlobalConfig.DeviceName, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "lox_udpport")
        strcpy(LoxoneConfig.UDPPort, WebServer.arg(i).c_str());
      if (WebServer.argName(i) == "rstate")
        GlobalConfig.restoreOldRelayState = (String(WebServer.arg(i)).toInt() == 1);
      if (WebServer.argName(i) == "leddisabled")
        GlobalConfig.LEDDisabled = (String(WebServer.arg(i)).toInt() == 1);

    }
    if (sc) {
      setLastState(digitalRead(RelayPin));
      saveSuccess = saveSystemConfig();
      if (GlobalConfig.BackendType == BackendType_HomeMatic) {
        String devName = getStateCUxD(GlobalConfig.DeviceName, "Address") ;
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
  String page = FPSTR(HTTP_HEAD);

  //page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_ALL_STYLE);
  if (GlobalConfig.BackendType == BackendType_HomeMatic)
    page += FPSTR(HTTP_HM_STYLE);
  if (GlobalConfig.BackendType == BackendType_Loxone)
    page += FPSTR(HTTP_LOX_STYLE);
  page += FPSTR(HTTP_HEAD_END);
  page += F("<div class='fbg'>");
  page += F("<form method='post' action='config'>");
  page += FPSTR(HTTP_TITLE_LABEL);
  page += FPSTR(HTTP_CONF);

  if (GlobalConfig.SonoffModel == SonoffModel_Switch) {
    page += FPSTR(HTTP_CONF_ADD_SWITCH);
  }

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    page += FPSTR(HTTP_CONF_POW_MEASURE_INTERVAL);
  }
  if (GlobalConfig.BackendType == BackendType_HomeMatic) {
    if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
      page += FPSTR(HTTP_CONF_HM_POW);
      page.replace("{hmpowvar}", HomeMaticConfig.PowerVariableName);
    }
    page.replace("{st}", "CCU2 IP");
    page.replace("{remanenz}", "Schaltzustand wiederherstellen");
  }
  if (GlobalConfig.BackendType == BackendType_Loxone) {
    page += FPSTR(HTTP_CONF_LOX);
    page.replace("{st}", "MiniServer IP");
    page.replace("{udp}", LoxoneConfig.UDPPort);
    page.replace("{remanenz}", "Remanenzeingang");
  }

  page.replace("{rs}", ((GlobalConfig.restoreOldRelayState) ? "checked" : ""));
  page.replace("{le}", ((GlobalConfig.LEDDisabled) ? "checked" : ""));
  page.replace("{dn}", GlobalConfig.DeviceName);
  page.replace("{ccuip}", GlobalConfig.ccuIP);
  page.replace("{mi}", String(GlobalConfig.MeasureInterval));

  page += FPSTR(HTTP_STATUSLABEL);

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

  page += FPSTR(HTTP_SAVE_BUTTON);
  page += FPSTR(HTTP_HOME_BUTTON);
  page += FPSTR(HTTP_FW_LABEL);
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
  return "{\"state\": " + String(digitalRead(RelayPin)) + ", \"timer\": " + String(TimerSeconds) + ", \"resttimer\": " + String((TimerSeconds > 0) ? (TimerSeconds - (millis() - TimerStartMillis) / 1000) : 0) + "}";
}

void versionHtml() {
  WebServer.send(200, "text/plain", "<fw>" + FIRMWARE_VERSION + "</fw>");
}

void replyPowerJSON() {
  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    WebServer.send(200, "application/json", "{\"Voltage\":\"" + String(hlw8012value.voltage) + "\",\"Current\":\"" + String(hlw8012value.current) + "\",\"PowerW\":\"" + String(hlw8012value.powerw) + "\",\"PowerVA\":\"" + String(hlw8012value.powerva) + "\"}");
  else
    WebServer.send(200, "application/json", "{\"Voltage\":\"NaN\",\"Current\":\"NaN\",\"PowerW\":\"NaN\",\"PowerVA\":\"NaN\"}");
}

void replyPower() {
  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    WebServer.send(200, "text/plain", "<voltage>" + String(hlw8012value.voltage) + "</voltage><current>" + String(hlw8012value.current) + "</current><powerw>" + String(hlw8012value.powerw) + "</powerw><powerva>" + String(hlw8012value.powerva) + "</powerva>");
  else
    WebServer.send(200, "text/plain", "not supported");
}

