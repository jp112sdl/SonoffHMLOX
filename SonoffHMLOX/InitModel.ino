void initModelSwitch() {
  DEBUG("\nSonoff Modell = Switch / S20");
  SwitchPin = SonoffSwitchPin;
  LEDPin = LEDPinSwitch;
  On = LOW;
  Off = HIGH;
  pinMode(SwitchGPIOPin14, INPUT_PULLUP);
}

void initModelPow() {
  DEBUG("\nSonoff Modell = POW");
  SwitchPin = SonoffSwitchPin;
  LEDPin = LEDPinPow;
  On = HIGH;
  Off = LOW;
  hlw_init();
  GlobalConfig.GPIO14Mode = GPIO14Mode_OFF;
}

void initModelTouchAsSender() {
  DEBUG("\nSonoff Modell = Touch as Sender");
  SwitchPin = SonoffSwitchPin;
  LEDPin = LEDPinSwitch;
  On = LOW;
  Off = HIGH;
  GlobalConfig.GPIO14Mode = GPIO14Mode_OFF;
}

void initModelObi() {
  DEBUG("\nSonoff Modell = Obi Zwischenstecker");
  LEDPin = LEDPinObi;
  pinMode(ObiRelayOffPin,  OUTPUT);
  SwitchPin = ObiSwitchPin;
  On = HIGH;
  Off = LOW;
  GlobalConfig.GPIO14Mode = GPIO14Mode_OFF;
}

