void sethlwInterrupts() {
  attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
  attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

void hlwcalibrate() {
  unsigned long timeout = millis();
  while ((millis() - timeout) < 10000) {
    delay(1);
  }
  hlw8012.expectedActivePower(60.0);
  hlw8012.expectedVoltage(230.0);
  hlw8012.expectedCurrent(60.0 / 230.0);
  Serial.print(F("[HLW] New current multiplier : ")); Serial.println(hlw8012.getCurrentMultiplier());
  Serial.print(F("[HLW] New voltage multiplier : ")); Serial.println(hlw8012.getVoltageMultiplier());
  Serial.print(F("[HLW] New power multiplier   : ")); Serial.println(hlw8012.getPowerMultiplier());
}

void hlw_init() {
  hlw8012.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true);
  hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);
  Serial.print(F("[HLW] Default current multiplier : ")); Serial.println(hlw8012.getCurrentMultiplier());
  Serial.print(F("[HLW] Default voltage multiplier : ")); Serial.println(hlw8012.getVoltageMultiplier());
  Serial.print(F("[HLW] Default power multiplier   : ")); Serial.println(hlw8012.getPowerMultiplier());
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
    Serial.print(F("[HLW] Active Power (W)    : ")); Serial.println(hlw8012value.powerw);
    Serial.print(F("[HLW] Voltage (V)         : ")); Serial.println(hlw8012value.voltage);
    Serial.print(F("[HLW] Current (A)         : ")); Serial.println(hlw8012value.current);
    Serial.print(F("[HLW] Apparent Power (VA) : ")); Serial.println(hlw8012value.powerva);
    Serial.print(F("[HLW] Power Factor (%)    : ")); Serial.println((int) (100 * hlw8012.getPowerFactor()));
    Serial.println();
    if (GlobalConfig.BackendType == BackendType_HomeMatic) {
      if (String(HomeMaticConfig.PowerVariableName) != "") {
        setStateCUxD(String(HomeMaticConfig.PowerVariableName), String(hlw8012value.powerw));
      }
    }
  }
}

