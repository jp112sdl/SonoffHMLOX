/*
  Generic ESP8285 Module
  Flash Mode: DOUT
  Flash Frequency: 40 MHz
  CPU Frequency: 80 MHz
  Flash Size: 1M (64k SPIFFS)
  esp8266/arduino core 2.4.x - working 16.02.5328a8b
*/
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

const String FIRMWARE_VERSION = "1.0.23";
//#define                       UDPDEBUG
#define                       SERIALDEBUG

#define LEDPinSwitch          13
#define LEDPinPow             15
#define SEL_PIN                5
#define CF1_PIN               13
#define CF_PIN                14
#define RelayPin              12
#define SwitchPin              0
#define SwitchGPIOPin14       14
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

enum GPIO14Modes_e {
  GPIO14Mode_OFF,
  GPIO14Mode_KEY,
  GPIO14Mode_SWITCH_ABSOLUT,
  GPIO14Mode_SWITCH_TOGGLE
};

struct globalconfig_t {
  char ccuIP[IPSIZE]   = "";
  char DeviceName[VARIABLESIZE] = "";
  bool restoreOldRelayState = false;
  bool lastRelayState = false;
  int  MeasureInterval = 10;
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
      pinMode(SwitchGPIOPin14, INPUT_PULLUP);
      break;
    case SonoffModel_Pow:
      DEBUG("\nSonoff Modell = POW");
      LEDPin = LEDPinPow;
      On = HIGH;
      Off = LOW;
      hlw_init();
      GlobalConfig.GPIO14Mode = GPIO14Mode_OFF;
      break;
    case SonoffModel_TouchAsSender:
      DEBUG("\nSonoff Modell = Touch as Sender");
      LEDPin = 13;
      On = LOW;
      Off = HIGH;
      GlobalConfig.GPIO14Mode = GPIO14Mode_OFF;
      break;
  }

  pinMode(LEDPin, OUTPUT);

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

  if (GlobalConfig.SonoffModel == SonoffModel_Pow)
    switchLED(On);

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
  if ((GlobalConfig.restoreOldRelayState) && GlobalConfig.lastRelayState == true) {
    switchRelay(RELAYSTATE_ON, TRANSMITSTATE);
  } else {
    switchRelay(RELAYSTATE_OFF, TRANSMITSTATE);
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

    CurrentSwitchGPIO14State = digitalRead(SwitchGPIOPin14);
    //GPIO14 als Schalter
    if (GlobalConfig.GPIO14Mode == GPIO14Mode_SWITCH_ABSOLUT || GlobalConfig.GPIO14Mode == GPIO14Mode_SWITCH_TOGGLE) {
      if (CurrentSwitchGPIO14State != LastSwitchGPIOPin14State) {
        DEBUG("GPIO14 neuer Status = " + String(CurrentSwitchGPIO14State), "loop()", _slInformational);
        LastSwitchGPIOPin14State = CurrentSwitchGPIO14State;
        if (GlobalConfig.GPIO14asSender) {
          if (GlobalConfig.GPIO14Mode == GPIO14Mode_SWITCH_ABSOLUT) {
            if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelNameSender + ".SET_STATE",  (!CurrentSwitchGPIO14State ? "1" : "0"));
          }
          if (GlobalConfig.GPIO14Mode == GPIO14Mode_SWITCH_TOGGLE) {
            if (GlobalConfig.BackendType == BackendType_HomeMatic) {
              String currentState = getStateCUxD(HomeMaticConfig.ChannelNameSender + ".STATE", "State()");
              DEBUG("CUxD Switch currentState = " + String(currentState));
              setStateCUxD(HomeMaticConfig.ChannelNameSender + ".SET_STATE",  (currentState == "false" ? "1" : "0"));
            }
          }

        } else {
          if (GlobalConfig.GPIO14Mode == GPIO14Mode_SWITCH_ABSOLUT)
            switchRelay(!CurrentSwitchGPIO14State, TRANSMITSTATE); //HIGH = off, LOW = on
          if (GlobalConfig.GPIO14Mode == GPIO14Mode_SWITCH_TOGGLE)
            toggleRelay(TRANSMITSTATE);
        }
      }
    }

    //Tasterbedienung am Sonoff abarbeiten
    if (digitalRead(SwitchPin) == LOW || (GlobalConfig.GPIO14Mode == GPIO14Mode_KEY && CurrentSwitchGPIO14State == LOW)) {
      if (!KeyPress) {
        KeyPressDownMillis = millis();
        if (millis() - LastMillisKeyPress > MillisKeyBounce) {
          LastMillisKeyPress = millis();
          if (GlobalConfig.SonoffModel != SonoffModel_TouchAsSender && !GlobalConfig.GPIO14asSender) {
            toggleRelay(TRANSMITSTATE);
          } else {
            switchLED(On);
          }
          KeyPress = true;
        }
      }

      if ((GlobalConfig.SonoffModel == SonoffModel_TouchAsSender || GlobalConfig.GPIO14asSender) && (millis() - KeyPressDownMillis) > KEYPRESSLONGMILLIS && !PRESS_LONGsent) {
        //PRESS_LONG
        DEBUG("Touch or GPIO14 as Sender: PRESS_LONG", "loop()", _slInformational);
        if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelNameSender + ".PRESS_LONG", "true");
        if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + ":1 = PRESS_LONG");
        switchLED(Off);
        PRESS_LONGsent = true;
      }

    } else {
      if (GlobalConfig.SonoffModel == SonoffModel_TouchAsSender || GlobalConfig.GPIO14asSender) {
        if (KeyPress) {
          if ((millis() - KeyPressDownMillis) < KEYPRESSLONGMILLIS) {
            //PRESS_SHORT
            DEBUG("Touch or GPIO14 as Sender: PRESS_SHORT", "loop()", _slInformational);
            if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelNameSender + ".PRESS_SHORT", "true");
            if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + ":1 = PRESS_SHORT");
          }
          switchLED(Off);
        }
      }
      KeyPress = false;
      PRESS_LONGsent = false;
    }

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

void switchRelay(bool toState, bool transmitState) {
  RelayState = toState;
  DEBUG("Switch Relay to " + String(toState) + " with transmitState = " + String(transmitState), "switchRelay()", _slInformational);

  if (toState == RELAYSTATE_OFF) {
    TimerSeconds = 0;
  }

  digitalWrite(RelayPin, RelayState);
  setLastRelayState(RelayState);

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
