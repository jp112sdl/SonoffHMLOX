/*
  Generic ESP8266 Module
  Flash Mode: DOUT
  Flash Frequency: 40 MHz
  CPU Frequency: 80 MHz
  Flash Size: 1M (64k SPIFFS)
  Debug Port: disabled
  Debug Level: none
  Reset Mode: ck
  Upload Speed: 115200
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
#define PING_ENABLED        true
#define PINGINTERVALSECONDS  300

const String FIRMWARE_VERSION = "1.0.4";
const char GITHUB_SSL_FINGERPRINT[] PROGMEM = "35:85:74:EF:67:35:A7:CE:40:69:50:F3:C0:F6:80:CF:80:3B:2E:19";
const char GITHUB_REPO_URL[] PROGMEM = "https://api.github.com/repos/jp112sdl/SonoffHMLOX/releases/latest";

enum BackendTypes_e {
  BackendType_HomeMatic,
  BackendType_Loxone
};

enum SonoffModel_e {
  SonoffModel_Switch,
  SonoffModel_Pow
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
  byte FirmwareCheckIntervalHours = 24;
  byte SonoffModel = SonoffModel_Switch;
  String Hostname = "Sonoff";
} GlobalConfig;

struct hmconfig_t {
  String ChannelName = "";
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
unsigned long LastFirmwareCheckMillis = 0;
unsigned long LastHlwMeasureMillis = 0;
unsigned long LastHlwCollectMillis = 0;
unsigned long LastPingMillis = 0;
int TimerSeconds = 0;
bool OTAStart = false;
bool newFirmwareAvailable = false;
bool startWifiManager = false;
bool wm_shouldSaveConfig        = false;
#define wifiManagerDebugOutput   true

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
#define HLWDISCARDNUM                   4  //Wieviele Werte sollen verworfen werden
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

  Serial.print(F("Config-Modus durch bootConfigMode aktivieren? "));
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

  if (doWifiConnect()) {
    Serial.println(F("WLAN erfolgreich verbunden!"));
    printWifiStatus();
  } else ESP.restart();


  switch (GlobalConfig.SonoffModel) {
    case SonoffModel_Switch:
      Serial.println("\nSonoff Modell = Switch / S20");
      LEDPin = 13;
      On = LOW;
      Off = HIGH;
      break;
    case SonoffModel_Pow:
      Serial.println("\nSonoff Modell = POW");
      LEDPin = 15;
      On = HIGH;
      Off = LOW;
      hlw_init();
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
    Serial.println("Error setting up MDNS responder!");
  }

  GlobalConfig.lastRelayState = getLastState();

  digitalWrite(LEDPin, (GlobalConfig.SonoffModel == SonoffModel_Pow));

  if (GlobalConfig.BackendType == BackendType_HomeMatic) {
    HomeMaticConfig.ChannelName =  "CUxD." + getStateCUxD(GlobalConfig.DeviceName, "Address");
    if ((GlobalConfig.restoreOldRelayState) && GlobalConfig.lastRelayState == true) {
      switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
    } else {
      switchRelay(RELAYSTATE_OFF, (getStateCUxD(HomeMaticConfig.ChannelName + ".STATE", "State") == "true"));
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

  Serial.println("Starte UDP-Handler an Port " + String(UDPPORT) + "...");
  UDPClient.UDP.begin(UDPPORT);
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
  if (LastFirmwareCheckMillis > millis())
    LastFirmwareCheckMillis = millis();

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
      Serial.println("webSwitchRelayOn(), Timer aktiviert, Sekunden: " + String(TimerSeconds));
    } else {
      Serial.println(F("webSwitchRelayOn(), Parameter, aber mit TimerSeconds = 0"));
    }
    switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
  }

  //eingehende HTTP Anfragen abarbeiten
  WebServer.handleClient();

  //Tasterbedienung am Sonoff abarbeiten
  if (digitalRead(SwitchPin) == LOW) {
    if (KeyPress == false) {
      if (millis() - LastMillisKeyPress > MillisKeyBounce) {
        LastMillisKeyPress = millis();
        TimerSeconds = 0;
        toggleRelay(TRANSMITSTATE);
        KeyPress = true;
      }
    }
  } else {
    KeyPress = false;
  }

  //Timer
  if (TimerSeconds > 0 && millis() - TimerStartMillis > TimerSeconds * 1000) {
    Serial.println(F("Timer abgelaufen. Schalte Relais aus."));
    switchRelay(RELAYSTATE_OFF, TRANSMITSTATE);
  }
  if (LastFirmwareCheckMillis == 0 || millis() - LastFirmwareCheckMillis > GlobalConfig.FirmwareCheckIntervalHours * 1000 * 60 * 60) {
    LastFirmwareCheckMillis = millis();
    newFirmwareAvailable = checkGithubForNewFWVersion();
  }

  if (PING_ENABLED && (LastPingMillis == 0 || millis() - LastPingMillis > PINGINTERVALSECONDS * 1000)) {
    LastPingMillis = millis();
    Serial.print("Ping Zentrale " + String(GlobalConfig.ccuIP) + " ... ");
    const char* ipStr = GlobalConfig.ccuIP; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
    IPAddress pingHost = IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
    bool ret = Ping.ping(pingHost);
    Serial.println((ret) ? "success" : "fail");
  }

  //POW Handling
  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    handleHLW8012();

  //needed for UDP packet parser
  delay(10);
}

void switchRelay(bool toState, bool transmitState) {
  RelayState = toState;
  Serial.println("Switch Relay to " + String(toState) + " with transmitState = " + String(transmitState));

  if (toState == RELAYSTATE_OFF) {
    TimerSeconds = 0;
  }

  digitalWrite(RelayPin, RelayState);
  setLastState(RelayState);

  if (transmitState) {
    if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelName + ".SET_STATE", String(RelayState));
    if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + "=" + String(RelayState));
  }

  if (GlobalConfig.SonoffModel == SonoffModel_Switch) {
    digitalWrite(LEDPin, (RelayState ? On : Off));
  }

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    LastHlwCollectMillis = millis();
    LastHlwMeasureMillis = millis();
  }
}

void toggleRelay(bool transmitState) {
  if (digitalRead(RelayPin) == LOW) {
    switchRelay(RELAYSTATE_ON, transmitState);
  } else  {
    switchRelay(RELAYSTATE_OFF, transmitState);
  }
}

void blinkLED(int count) {
  byte oldState = digitalRead(LEDPin);
  delay(100);
  for (int i = 0; i < count; i++) {
    digitalWrite(LEDPin, !oldState);
    delay(100);
    digitalWrite(LEDPin, oldState);
    delay(100);
  }
  delay(200);
}
