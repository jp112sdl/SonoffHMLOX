void switchLED(bool State) {
  if ((GlobalConfig.SonoffModel == SonoffModel_Switch || GlobalConfig.SonoffModel == SonoffModel_TouchAsSender || GlobalConfig.SonoffModel == SonoffModel_ObiZwischenstecker) && GlobalConfig.LEDDisabled) {
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
