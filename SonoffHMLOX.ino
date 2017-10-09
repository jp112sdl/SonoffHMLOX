/*
  Generic ESP8285 Module
  Flash Mode: DOUT
  Flash Frequency: 40 MHz
  CPU Frequency: 80 MHz
  Flash Size: 1M (64k SPIFFS)
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "WM.h"
#include <FS.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <HLW8012.h>
#include <Arduino.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266Ping.h>
#include <ESP8266mDNS.h>
#include "css_global.h"
#include "js_global.h"
#include "js_pow.h"
#include "js_fwupd.h"

const String FIRMWARE_VERSION = "1.0.10";
//#define                       UDPDEBUG
//#define                       SERIALDEBUG

#define LEDPinSwitch          13
#define LEDPinPow             15
#define SEL_PIN                5
#define CF1_PIN               13
#define CF_PIN                14
#define RelayPin              12
#define SwitchPin              0
#define MillisKeyBounce      500  //Millisekunden zwischen 2xtasten
#define ConfigPortalTimeout  180  //Timeout (Sekunden) des AccessPoint-Modus
#define HTTPTimeOut         3000  //Timeout (Millisekunden) für http requests
#define IPSIZE                16
#define VARIABLESIZE         255
#define UDPPORT             6676
#define PING_ENABLED        false
#define PINGINTERVALSECONDS  300
#define KEYPRESSLONGMILLIS  1500 //Millisekunden für langen Tastendruck bei Sonoff Touch als Sender

const char GITHUB_REPO_URL[] PROGMEM = "https://api.github.com/repos/jp112sdl/SonoffHMLOX/releases/latest";

#ifdef UDPDEBUG
const char * SYSLOGIP = "192.168.1.251";
#define SYSLOGPORT          514
#endif

enum BackendTypes_e {
  BackendType_HomeMatic,
  BackendType_Loxone
};

enum SonoffModel_e {
  SonoffModel_Switch,
  SonoffModel_Pow,
  SonoffModel_TouchAsSender
};

enum RelayStates_e {
  RELAYSTATE_OFF,
  RELAYSTATE_ON
};

enum TransmitStates_e {
  NO_TRANSMITSTATE,
  TRANSMITSTATE
};

struct globalconfig_t {
  char ccuIP[IPSIZE]   = "";
  char DeviceName[VARIABLESIZE] = "";
  bool restoreOldRelayState = false;
  bool lastRelayState = false;
  int  MeasureInterval  = 10;
  byte BackendType = BackendType_HomeMatic;
  byte SonoffModel = SonoffModel_Switch;
  String Hostname = "Sonoff";
  bool LEDDisabled = false;
} GlobalConfig;

struct hmconfig_t {
  String ChannelName = "";
  String ChannelNameSender = "";
  char PowerVariableName[VARIABLESIZE] = "";
} HomeMaticConfig;

struct loxoneconfig_t {
  char Username[VARIABLESIZE] = "";
  char Password[VARIABLESIZE] = "";
  char UDPPort[10] = "";
} LoxoneConfig;

struct sonoffnetconfig_t {
  char ip[IPSIZE]      = "0.0.0.0";
  char netmask[IPSIZE] = "0.0.0.0";
  char gw[IPSIZE]      = "0.0.0.0";
} SonoffNetConfig;

enum _SyslogSeverity {
  _slEmergency,
  _slAlert,
  _slCritical,
  _slError,
  _slWarning,
  _slNotice,
  _slInformational,
  _slDebug
};

const String bootConfigModeFilename = "bootcfg.mod";
const String lastRelayStateFilename = "laststat.txt";
const String configJsonFile         = "config.json";
bool RelayState = LOW;
bool KeyPress = false;
byte LEDPin = 13;
byte On = 1;
byte Off = 0;
unsigned long LastMillisKeyPress = 0;
unsigned long TimerStartMillis = 0;
unsigned long LastHlwMeasureMillis = 0;
unsigned long LastHlwCollectMillis = 0;
unsigned long LastPingMillis = 0;
unsigned long KeyPressDownMillis = 0;
int TimerSeconds = 0;
bool OTAStart = false;
bool UDPReady = false;
bool newFirmwareAvailable = false;
bool startWifiManager = false;
bool wm_shouldSaveConfig        = false;
bool PRESS_LONGsent = false;
#define wifiManagerDebugOutput   false

ESP8266WebServer WebServer(80);
ESP8266HTTPUpdateServer httpUpdater;

struct udp_t {
  WiFiUDP UDP;
  char incomingPacket[255];
} UDPClient;

//HLW8012
#define CURRENT_MODE                    HIGH
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 5 * 470000 ) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000 ) // Real 1.009k
#define defaultCurrentMultiplier        13670.9
#define defaultVoltageMultiplier        441250.69
#define defaultPowerMultiplier          12168954.98
#define HLWMAXCOLLECTCOUNT              20 //Anzahl Werte für Mittelwertbildung
#define HLWDISCARDNUM                   6  //Wieviele Werte sollen verworfen werden
#define HLWCOLLECTINTERVAL              500 //ms

struct hlwvalues_ {
  float ActivePower[HLWMAXCOLLECTCOUNT];
  float ApparentPower[HLWMAXCOLLECTCOUNT];
  float Voltage[HLWMAXCOLLECTCOUNT];
  float Current[HLWMAXCOLLECTCOUNT];
  int HlwCollectCounter = 0;
} hlwvalues;

struct hlw8012value_t {
  float voltage = 0;
  float current = 0;
  float powerw  = 0;
  float powerva = 0;
} hlw8012value;

struct hlw8012calibrationdata_t {
  float CurrentMultiplier = defaultCurrentMultiplier;
  float VoltageMultiplier = defaultVoltageMultiplier;
  float PowerMultiplier  = defaultPowerMultiplier;
} HLW8012Calibration;

HLW8012 hlw8012;

void ICACHE_RAM_ATTR hlw8012_cf1_interrupt() {
  hlw8012.cf1_interrupt();
}
void ICACHE_RAM_ATTR hlw8012_cf_interrupt() {
  hlw8012.cf_interrupt();
}
//

void setup() {
  Serial.begin(115200);
  Serial.println("\nSonoff " + WiFi.macAddress() + " startet...");
  pinMode(LEDPinSwitch, OUTPUT);
  pinMode(LEDPinPow,    OUTPUT);
  pinMode(RelayPin,     OUTPUT);
  pinMode(SwitchPin,    INPUT_PULLUP);

  Serial.println(F("Config-Modus durch bootConfigMode aktivieren? "));
  if (SPIFFS.begin()) {
    Serial.println(F("-> bootConfigModeFilename mounted file system"));
    if (SPIFFS.exists("/" + bootConfigModeFilename)) {
      startWifiManager = true;
      Serial.println("-> " + bootConfigModeFilename + " existiert, starte Config-Modus");
      SPIFFS.remove("/" + bootConfigModeFilename);
      SPIFFS.end();
    } else {
      Serial.println("-> " + bootConfigModeFilename + " existiert NICHT");
    }
  } else {
    Serial.println(F("-> Nein, SPIFFS mount fail!"));
  }

  if (!startWifiManager) {
    Serial.println(F("Config-Modus mit Taster aktivieren?"));
    for (int i = 0; i < 20; i++) {
      if (digitalRead(SwitchPin) == LOW) {
        startWifiManager = true;
        break;
      }
      digitalWrite(LEDPinSwitch, HIGH);
      digitalWrite(LEDPinPow, LOW);
      delay(100);
      digitalWrite(LEDPinSwitch, LOW);
      digitalWrite(LEDPinPow, HIGH);
      delay(100);
    }
    Serial.println("Config-Modus " + String(((startWifiManager) ? "" : "nicht ")) + "aktiviert.");
  }

  if (!loadSystemConfig()) startWifiManager = true;
  //Ab hier ist die Config geladen und alle Variablen sind mit deren Werten belegt!

  if (doWifiConnect()) {
    Serial.println(F("\nWLAN erfolgreich verbunden!"));
    printWifiStatus();
  } else ESP.restart();

#ifndef SERIALDEBUG
  Serial.println("\nSerieller Debug nicht konfiguriert. Deshalb ist hier jetzt ENDE.\n");
  delay(20); //to flush serial buffer
  Serial.end();
#endif

  switch (GlobalConfig.SonoffModel) {
    case SonoffModel_Switch:
      DEBUG("\nSonoff Modell = Switch / S20");
      LEDPin = 13;
      On = LOW;
      Off = HIGH;
      break;
    case SonoffModel_Pow:
      DEBUG("\nSonoff Modell = POW");
      LEDPin = 15;
      On = HIGH;
      Off = LOW;
      hlw_init();
      break;
    case SonoffModel_TouchAsSender:
      DEBUG("\nSonoff Modell = Touch as Sender");
      LEDPin = 13;
      On = LOW;
      Off = HIGH;
      break;
  }

  pinMode(LEDPin, OUTPUT);

  WebServer.on("/0", webSwitchRelayOff);
  WebServer.on("/1", webSwitchRelayOn);
  WebServer.on("/2", webToggleRelay);
  WebServer.on("/toggle", webToggleRelay);
  WebServer.on("/getState", replyRelayState);
  WebServer.on("/bootConfigMode", setBootConfigMode);
  WebServer.on("/reboot", []() {
    WebServer.send(200, "text/plain", "rebooting");
    ESP.restart();
  });
  WebServer.on("/restart", []() {
    WebServer.send(200, "text/plain", "rebooting");
    ESP.restart();
  });
  WebServer.on("/version", versionHtml);
  WebServer.on("/firmware", versionHtml);
  WebServer.on("/config", configHtml);
  WebServer.on("/calibrate", calibrateHtml);
  WebServer.on("/getPower", replyPower);
  WebServer.on("/getPowerJSON", replyPowerJSON);
  httpUpdater.setup(&WebServer);
  WebServer.onNotFound(defaultHtml);

  WebServer.begin();

  if (!MDNS.begin(GlobalConfig.Hostname.c_str())) {
    DEBUG("Error setting up MDNS responder!");
  }

  GlobalConfig.lastRelayState = getLastState();

  switchLED(GlobalConfig.SonoffModel == SonoffModel_Pow);

  if (GlobalConfig.BackendType == BackendType_HomeMatic) {
    HomeMaticConfig.ChannelName =  "CUxD." + getStateCUxD(GlobalConfig.DeviceName, "Address");
    DEBUG("HomeMaticConfig.ChannelName =  " + HomeMaticConfig.ChannelName);
    if (GlobalConfig.restoreOldRelayState && GlobalConfig.lastRelayState == true) {
      switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
    } else {
      switchRelay(RELAYSTATE_OFF, (getStateCUxD(HomeMaticConfig.ChannelName + ".STATE", "State") == "true"));
    }

    if (GlobalConfig.SonoffModel == SonoffModel_TouchAsSender) {
      HomeMaticConfig.ChannelNameSender =  "CUxD." + getStateCUxD(String(GlobalConfig.DeviceName) + ":1", "Address");
      DEBUG("HomeMaticConfig.ChannelNameSender =  " + HomeMaticConfig.ChannelNameSender);
    }
  }

  if (GlobalConfig.BackendType == BackendType_Loxone) {
    if ((GlobalConfig.restoreOldRelayState) && GlobalConfig.lastRelayState == true) {
      switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
    } else {
      switchRelay(RELAYSTATE_OFF, NO_TRANSMITSTATE);
    }
  }

  startOTAhandling();

  DEBUG("Starte UDP-Handler an Port " + String(UDPPORT) + "...");
  UDPClient.UDP.begin(UDPPORT);
  UDPReady = true;
  DEBUG(String(GlobalConfig.DeviceName) + " - Boot abgeschlossen, SSID = " + WiFi.SSID() + ", IP = " + String(IpAddress2String(WiFi.localIP())) + ", RSSI = " + WiFi.RSSI() + ", MAC = " + WiFi.macAddress(), "Setup", _slInformational);
}

void loop() {
  //Überlauf der millis() abfangen
  if (LastMillisKeyPress > millis())
    LastMillisKeyPress = millis();
  if (TimerStartMillis > millis())
    TimerStartMillis = millis();
  if (LastHlwMeasureMillis > millis())
    LastHlwMeasureMillis = millis();
  if (LastHlwCollectMillis > millis())
    LastHlwCollectMillis = millis();
  if (LastPingMillis > millis())
    LastPingMillis = millis();

  //auf OTA Anforderung reagieren
  ArduinoOTA.handle();

  //eingehende UDP Kommandos abarbeiten
  String udpMessage = handleUDP();
  if (udpMessage == "bootConfigMode")
    setBootConfigMode;
  if (udpMessage == "reboot")
    ESP.restart();
  if (udpMessage == "1" || udpMessage == "on")
    switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
  if (udpMessage == "0" || udpMessage == "off")
    switchRelay(RELAYSTATE_OFF, NO_TRANSMITSTATE);
  if (udpMessage == "2" || udpMessage == "toggle")
    toggleRelay(false);
  if (udpMessage.indexOf("1?t=") != -1) {
    TimerSeconds = (udpMessage.substring(4, udpMessage.length())).toInt();
    if (TimerSeconds > 0) {
      TimerStartMillis = millis();
      DEBUG("webSwitchRelayOn(), Timer aktiviert, Sekunden: " + String(TimerSeconds), "loop()", _slInformational);
    } else {
      DEBUG(F("webSwitchRelayOn(), Parameter, aber mit TimerSeconds = 0"), "loop()", _slInformational);
    }
    switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
  }

  //eingehende HTTP Anfragen abarbeiten
  WebServer.handleClient();

  //Tasterbedienung am Sonoff abarbeiten
  if (digitalRead(SwitchPin) == LOW) {
    if (!KeyPress) {
      KeyPressDownMillis = millis();
      if (millis() - LastMillisKeyPress > MillisKeyBounce) {
        LastMillisKeyPress = millis();
        if (GlobalConfig.SonoffModel != SonoffModel_TouchAsSender) {
          toggleRelay(TRANSMITSTATE);
        } else {
          switchLED(On);
        }
        KeyPress = true;
      }
    }

    if ((millis() - KeyPressDownMillis) > KEYPRESSLONGMILLIS && !PRESS_LONGsent) {
      //PRESS_LONG
      DEBUG("Touch As Sender: PRESS_LONG", "loop()", _slInformational);
      if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelNameSender + ".PRESS_LONG", "true");
      if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + ":1 = PRESS_LONG");
      switchLED(Off);
      PRESS_LONGsent = true;
    }

  } else {
    if (GlobalConfig.SonoffModel == SonoffModel_TouchAsSender) {
      if (KeyPress) {
        if ((millis() - KeyPressDownMillis) < KEYPRESSLONGMILLIS) {
          //PRESS_SHORT
          DEBUG("Touch As Sender: PRESS_SHORT", "loop()", _slInformational);
          if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelNameSender + ".PRESS_SHORT", "true");
          if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + ":1 = PRESS_SHORT");
        }
      }
    }
    KeyPress = false;
    PRESS_LONGsent = false;
    switchLED(Off);
  }

  //Timer
  if (TimerSeconds > 0 && millis() - TimerStartMillis > TimerSeconds * 1000) {
    DEBUG(F("Timer abgelaufen. Schalte Relais aus."), "loop()", _slInformational);
    switchRelay(RELAYSTATE_OFF, TRANSMITSTATE);
  }

  if (PING_ENABLED && (LastPingMillis == 0 || millis() - LastPingMillis > PINGINTERVALSECONDS * 1000)) {
    LastPingMillis = millis();
    DEBUG("Ping Zentrale " + String(GlobalConfig.ccuIP) + " ... ", "loop()", _slInformational);
    const char* ipStr = GlobalConfig.ccuIP; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
    IPAddress pingHost = IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
    bool ret = Ping.ping(pingHost);
    DEBUG((ret) ? "success" : "fail", "loop()", _slInformational);
  }

  //POW Handling
  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    handleHLW8012();

  //needed for UDP packet parser
  delay(10);
}

void switchRelay(bool toState, bool transmitState) {
  RelayState = toState;
  DEBUG("Switch Relay to " + String(toState) + " with transmitState = " + String(transmitState), "switchRelay()", _slInformational);

  if (toState == RELAYSTATE_OFF) {
    TimerSeconds = 0;
  }

  digitalWrite(RelayPin, RelayState);
  setLastState(RelayState);

  if (transmitState) {
    if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelName + ".SET_STATE", String(RelayState));
    if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + "=" + String(RelayState));
  }

  switchLED((RelayState ? On : Off));

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    LastHlwCollectMillis = millis();
    LastHlwMeasureMillis = millis();
  }
}

bool getRelayState() {
  return (digitalRead(RelayPin) == RELAYSTATE_ON);
}

void toggleRelay(bool transmitState) {
  TimerSeconds = 0;
  if (digitalRead(RelayPin) == LOW) {
    switchRelay(RELAYSTATE_ON, transmitState);
  } else  {
    switchRelay(RELAYSTATE_OFF, transmitState);
  }
}

void switchLED(bool State) {
  if ((GlobalConfig.SonoffModel == SonoffModel_Switch || GlobalConfig.SonoffModel == SonoffModel_TouchAsSender) && GlobalConfig.LEDDisabled) {
    digitalWrite(LEDPin, Off);
  } else {
    digitalWrite(LEDPin, State);
  }
}

void blinkLED(int count) {
  byte oldState = digitalRead(LEDPin);
  delay(100);
  for (int i = 0; i < count; i++) {
    switchLED(!oldState);
    delay(100);
    switchLED(oldState);
    delay(100);
  }
  delay(200);
}

String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") + \
         String(ipAddress[1]) + String(".") + \
         String(ipAddress[2]) + String(".") + \
         String(ipAddress[3]);
}
