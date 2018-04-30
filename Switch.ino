void gpio14Handling() {
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
}

void switchHandling() {
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
}
