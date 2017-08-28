void sethlwInterrupts() {
  attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
  attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

void hlwundocalibrate() {
  HLW8012Calibration.CurrentMultiplier = defaultCurrentMultiplier;
  HLW8012Calibration.VoltageMultiplier = defaultVoltageMultiplier;
  HLW8012Calibration.PowerMultiplier = defaultPowerMultiplier;

  hlw8012.setCurrentMultiplier(HLW8012Calibration.CurrentMultiplier);
  hlw8012.setVoltageMultiplier(HLW8012Calibration.VoltageMultiplier);
  hlw8012.setPowerMultiplier(HLW8012Calibration.PowerMultiplier);

  saveSystemConfig();
}

void hlwcalibrate(byte exp_voltage, byte exp_power ) {
  bool tmpOldState = RELAYSTATE_ON;
  if (RelayState == RELAYSTATE_OFF) {
    tmpOldState = RELAYSTATE_OFF;
    switchRelay(RELAYSTATE_ON, NO_TRANSMITSTATE);
  }

  delay(10000);

  hlw8012.expectedActivePower(exp_power);
  hlw8012.expectedVoltage(exp_voltage);
  hlw8012.expectedCurrent(exp_power / exp_power);

  delay(1000);

  float newCurrentMultiplier = hlw8012.getCurrentMultiplier();
  float newVoltageMultiplier = hlw8012.getVoltageMultiplier();
  float newPowerMultiplier = hlw8012.getPowerMultiplier();

  Serial.print(F("[HLW] New  multipliers : current = ")); Serial.print(String(newCurrentMultiplier));
  Serial.print(F(" / voltage = ")); Serial.print(String(newVoltageMultiplier));
  Serial.print(F(" / power = ")); Serial.print(String(newPowerMultiplier));
  Serial.println();

  HLW8012Calibration.CurrentMultiplier = newCurrentMultiplier;
  HLW8012Calibration.VoltageMultiplier = newVoltageMultiplier;
  HLW8012Calibration.PowerMultiplier = newPowerMultiplier;

  hlw8012.setCurrentMultiplier(HLW8012Calibration.CurrentMultiplier);
  hlw8012.setVoltageMultiplier(HLW8012Calibration.VoltageMultiplier);
  hlw8012.setPowerMultiplier(HLW8012Calibration.PowerMultiplier);

  saveSystemConfig();

  if (tmpOldState == RELAYSTATE_OFF)
    switchRelay(RELAYSTATE_OFF, NO_TRANSMITSTATE);
}

void hlw_init() {
  hlw8012.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true);
  hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);

  hlw8012.setCurrentMultiplier(HLW8012Calibration.CurrentMultiplier);
  hlw8012.setVoltageMultiplier(HLW8012Calibration.VoltageMultiplier);
  hlw8012.setPowerMultiplier(HLW8012Calibration.PowerMultiplier);

  Serial.print(F("[HLW] multipliers : current = ")); Serial.print(hlw8012.getCurrentMultiplier());
  Serial.print(F(" / voltage = ")); Serial.print(hlw8012.getVoltageMultiplier());
  Serial.print(F(" / power = ")); Serial.print(hlw8012.getPowerMultiplier());
  Serial.println();
  sethlwInterrupts();
}


void handleHLW8012() {
  if (!OTAStart && GlobalConfig.MeasureInterval > 0 &&  (millis() - LastHwlMillis) > (GlobalConfig.MeasureInterval * 1000)) {
    LastHwlMillis = millis();
    hlw8012value.powerw  = hlw8012.getActivePower();
    hlw8012value.powerva = hlw8012.getApparentPower();
    hlw8012value.voltage = hlw8012.getVoltage();
    hlw8012value.current = hlw8012.getCurrent();
    Serial.print(F("[HLW]: ")); Serial.print(hlw8012value.powerw);
    Serial.print(F("W, ")); Serial.print(hlw8012value.voltage);
    Serial.print(F("V, ")); Serial.print(hlw8012value.current);
    Serial.print(F("A, ")); Serial.print(hlw8012value.powerva);
    Serial.print(F("VA, Power Factor (%) : ")); Serial.print((int) (100 * hlw8012.getPowerFactor()));
    Serial.println();
    if (GlobalConfig.BackendType == BackendType_HomeMatic) {
      if (String(HomeMaticConfig.PowerVariableName) != "") {
        setStateCUxD(String(HomeMaticConfig.PowerVariableName), String(hlw8012value.powerw));
      }
    }
  }
}

