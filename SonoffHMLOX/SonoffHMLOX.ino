/*
  Generic ESP8285 Module
  Flash Mode: DOUT
  Flash Frequency: 40 MHz
  CPU Frequency: 80 MHz
  Flash Size: 1M (64k SPIFFS)
*/
#define PUYA_SUPPORT 1

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <HLW8012.h>
#include <WiFiUdp.h>
#include "WM.h"
#include "css_global.h"
#include "js_global.h"
#include "js_pow.h"
#include "js_fwupd.h"

const String FIRMWARE_VERSION = "1.0.28";
//#define                       UDPDEBUG
#define                       SERIALDEBUG

#define ObiRelayOffPin         5
#define LEDPinObi              4
#define ObiSwitchPin          14

#define SonoffSwitchPin        0
#define SwitchGPIOPin14       14

#define LEDPinSwitch          13
#define RelayPin              12
#define LEDPinPow             15
#define SEL_PIN                5
#define CF1_PIN               13
#define CF_PIN                14
#define MillisKeyBounce      100  //Millisekunden zwischen 2xtasten
#define ConfigPortalTimeout  180  //Timeout (Sekunden) des AccessPoint-Modus
#define HTTPTimeOut         1500  //Timeout (Millisekunden) für http requests
#define IPSIZE                16
#define VARIABLESIZE         255
#define UDPPORT             6676
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
  SonoffModel_TouchAsSender,
  SonoffModel_ObiZwischenstecker
};

enum RelayStates_e {
  RELAYSTATE_OFF,
  RELAYSTATE_ON
};

enum TransmitStates_e {
  NO_TRANSMITSTATE,
  TRANSMITSTATE
};

enum GPIO14Modes_e {
  GPIO14Mode_OFF,
  GPIO14Mode_KEY,
  GPIO14Mode_SWITCH_ABSOLUT,
  GPIO14Mode_SWITCH_TOGGLE
};

enum RelayStateOnBoot_e {
  RelayStateOnBoot_OFF,
  RelayStateOnBoot_LAST,
  RelayStateOnBoot_ON
};

struct globalconfig_t {
  char ccuIP[IPSIZE]   = "";
  char DeviceName[VARIABLESIZE] = "";
  uint8_t restoreOldRelayState = RelayStateOnBoot_OFF;
  bool lastRelayState = false;
  bool loadEcOnBoot = false;
  uint16_t  MeasureInterval = 10;
  byte BackendType = BackendType_HomeMatic;
  byte SonoffModel = SonoffModel_Switch;
  byte GPIO14Mode = GPIO14Mode_OFF;
  bool GPIO14asSender = false;
  String Hostname = "Sonoff";
  bool LEDDisabled = false;
} GlobalConfig;

struct hmconfig_t {
  String ChannelName = "";
  String ChannelNameSender = "";
  char PowerVariableName[VARIABLESIZE] = "";
  char EnergyCounterVariableName[VARIABLESIZE] = "";
  bool EnergyCounterVariableAvailable = false;
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
bool WiFiConnected = false;
bool LastSwitchGPIOPin14State = HIGH;
bool CurrentSwitchGPIO14State = HIGH;
byte LEDPin = 13;
byte SwitchPin = 0;
byte On = 1;
byte Off = 0;
unsigned long LastMillisKeyPress = 0;
unsigned long TimerStartMillis = 0;
unsigned long LastHlwMeasureMillis = 0;
unsigned long LastHlwCollectMillis = 0;
unsigned long KeyPressDownMillis = 0;
unsigned long TimerSeconds = 0;
unsigned long LastWiFiReconnectMillis = 0;
bool OTAStart = false;
bool UDPReady = false;
bool startWifiManager = false;
bool wm_shouldSaveConfig        = false;
bool PRESS_LONGsent = false;
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
  float voltage        = 0;
  float current        = 0;
  float powerw         = 0;
  float powerva        = 0;
  float energy_counter = 0;
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

void setup() {
  Serial.begin(115200);
  Serial.println("\nSonoff " + WiFi.macAddress() + " startet... (FW: " + FIRMWARE_VERSION + ")");
  pinMode(LEDPinSwitch,    OUTPUT);
  pinMode(LEDPinPow,       OUTPUT);
  pinMode(RelayPin,        OUTPUT);
  pinMode(LEDPinObi,       OUTPUT);
  pinMode(SonoffSwitchPin, INPUT_PULLUP);
  pinMode(ObiSwitchPin,    INPUT_PULLUP);

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

  if (!loadSystemConfig()) startWifiManager = true;
  //Ab hier ist die Config geladen und alle Variablen sind mit deren Werten belegt!

  if (!startWifiManager) {
    Serial.println(F("Config-Modus mit Taster aktivieren?"));
    for (int i = 0; i < 20; i++) {
      if (digitalRead(SwitchPin) == LOW || (GlobalConfig.SonoffModel != SonoffModel_Pow && digitalRead(ObiSwitchPin) == LOW)) {
        startWifiManager = true;
        break;
      }
      digitalWrite(LEDPinSwitch, HIGH);
      digitalWrite(LEDPinObi, LOW);
      digitalWrite(LEDPinPow, LOW);
      delay(100);
      digitalWrite(LEDPinSwitch, LOW);
      digitalWrite(LEDPinObi, HIGH);
      digitalWrite(LEDPinPow, HIGH);
      delay(100);
    }
    Serial.println("Config-Modus " + String(((startWifiManager) ? "" : "nicht ")) + "aktiviert.");
  }

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
      initModelSwitch();
      break;
    case SonoffModel_Pow:
      initModelPow();
      break;
    case SonoffModel_TouchAsSender:
      initModelTouchAsSender();
      break;
    case SonoffModel_ObiZwischenstecker:
      initModelObi();
      break;
  }

  pinMode(LEDPin, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);

  initWebServerHandler();

  httpUpdater.setup(&WebServer);
  WebServer.begin();

  if (!MDNS.begin(GlobalConfig.Hostname.c_str())) {
    DEBUG("Error setting up MDNS responder!");
  }

  startOTAhandling();

  DEBUG("Starte UDP-Handler an Port " + String(UDPPORT) + "...");
  UDPClient.UDP.begin(UDPPORT);
  UDPReady = true;

  if (GlobalConfig.BackendType == BackendType_HomeMatic) {
    reloadCUxDAddress(NO_TRANSMITSTATE);
    byte tryCount = 0;
    byte tryCountMax = 5;
    while (HomeMaticConfig.ChannelName == "CUxD.") {
      tryCount++;
      DEBUG("Failed getting CUxD Device from HomeMaticConfig.ChannelName. Retry " + String(tryCount) + " / " + String(tryCountMax));
      delay(1000);
      reloadCUxDAddress(NO_TRANSMITSTATE);
      if (tryCount == tryCountMax) break;
    }
  }

  GlobalConfig.lastRelayState = getLastRelayState();
  if (((GlobalConfig.restoreOldRelayState == RelayStateOnBoot_LAST) && GlobalConfig.lastRelayState == true) || GlobalConfig.restoreOldRelayState == RelayStateOnBoot_ON) {
    switchRelay(RELAYSTATE_ON, TRANSMITSTATE);
  } else {
    switchRelay(RELAYSTATE_OFF, TRANSMITSTATE);
  }

  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    switchLED(!GlobalConfig.LEDDisabled);
    HomeMaticConfig.EnergyCounterVariableAvailable = getEnergyCounterValueFromCCU(GlobalConfig.loadEcOnBoot);
  }

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
  if (LastWiFiReconnectMillis > millis())
    LastWiFiReconnectMillis = millis();

  //Reconnect WiFi wenn nicht verbunden (alle 30 Sekunden)
  if (WiFi.status() != WL_CONNECTED) {
    WiFiConnected = false;
    if (millis() - LastWiFiReconnectMillis > 30000) {
      LastWiFiReconnectMillis = millis();
      DEBUG("WiFi Connection lost! Reconnecting...");
      WiFi.reconnect();
    }
  } else {
    if (!WiFiConnected) {
      DEBUG("WiFi reconnected!");
      WiFiConnected = true;
    }
  }

  //auf OTA Anforderung reagieren
  ArduinoOTA.handle();

  if (!OTAStart) {
    //eingehende UDP Kommandos abarbeiten
    String udpMessage = handleUDP();
    if (udpMessage == "bootConfigMode")
      setBootConfigMode();
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

    //GPIO14 am Sonoff Switch
    gpio14Handling();

    //Tasterbedienung am Sonoff abarbeiten
    switchHandling();

    //Timer
    if (TimerSeconds > 0 && millis() - TimerStartMillis > TimerSeconds * 1000) {
      DEBUG(F("Timer abgelaufen. Schalte Relais aus."), "loop()", _slInformational);
      switchRelay(RELAYSTATE_OFF, TRANSMITSTATE);
    }

    //POW Handling
    if (GlobalConfig.SonoffModel == SonoffModel_Pow)
      handleHLW8012();

    //needed for UDP packet parser
    delay(10);
  }
}
