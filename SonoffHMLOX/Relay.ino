void switchRelay(bool toState, bool transmitState) {
  RelayState = toState;
  DEBUG("Switch Relay to " + String(toState) + " with transmitState = " + String(transmitState), "switchRelay()", _slInformational);

  if (toState == RELAYSTATE_OFF) {
    TimerSeconds = 0;
  }

  if (GlobalConfig.SonoffModel == SonoffModel_ObiZwischenstecker) {
    if (RelayState == RELAYSTATE_ON) {
      digitalWrite(RelayPin, LOW);
      digitalWrite(ObiRelayOffPin, HIGH);
    } else {
      digitalWrite(ObiRelayOffPin, LOW);
      digitalWrite(RelayPin, HIGH);
    }
  } else {
    digitalWrite(RelayPin, RelayState);
  }

  setLastRelayState(RelayState);

  if (transmitState) {
    if (GlobalConfig.BackendType == BackendType_HomeMatic) setStateCUxD(HomeMaticConfig.ChannelName + ".SET_STATE", String(RelayState));
    if (GlobalConfig.BackendType == BackendType_Loxone) sendLoxoneUDP(String(GlobalConfig.DeviceName) + "=" + String(RelayState));
  }


  if (GlobalConfig.SonoffModel == SonoffModel_Pow) {
    LastHlwCollectMillis = millis();
    LastHlwMeasureMillis = millis();
  }
  
  if (GlobalConfig.SonoffModel != SonoffModel_Pow) {
    switchLED((RelayState ? On : Off));
  }
}

bool getRelayState() {
  if (GlobalConfig.SonoffModel == SonoffModel_ObiZwischenstecker)
    return (digitalRead(RelayPin) == RELAYSTATE_OFF);
  else
    return (digitalRead(RelayPin) == RELAYSTATE_ON);
}

void toggleRelay(bool transmitState) {
  TimerSeconds = 0;
  if (getRelayState() == RELAYSTATE_OFF) {
    switchRelay(RELAYSTATE_ON, transmitState);
  } else  {
    switchRelay(RELAYSTATE_OFF, transmitState);
  }
}
