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

void hlwcalibrate(int exp_voltage, int exp_power ) {
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

  DEBUG("[HLW] New  multipliers : current = " + String(newCurrentMultiplier) +  " / voltage = " + String(newVoltageMultiplier) + " / power = " + String(newPowerMultiplier));

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

  DEBUG("[HLW] multipliers : current = " + String(hlw8012.getCurrentMultiplier()) + " / voltage = " + String(hlw8012.getVoltageMultiplier()) + " / power = " +  String(hlw8012.getPowerMultiplier()));
  sethlwInterrupts();
}

void handleHLW8012() {
  if (!OTAStart && millis() - LastHlwCollectMillis > HLWCOLLECTINTERVAL) {
    LastHlwCollectMillis = millis();
    if (getRelayState() == RELAYSTATE_ON) {
      if (hlwvalues.HlwCollectCounter >= HLWMAXCOLLECTCOUNT) {
        sort(hlwvalues.ActivePower, HLWMAXCOLLECTCOUNT);
        sort(hlwvalues.ApparentPower, HLWMAXCOLLECTCOUNT);
        sort(hlwvalues.Voltage, HLWMAXCOLLECTCOUNT);
        sort(hlwvalues.Current, HLWMAXCOLLECTCOUNT);

        float wtemp = 0, vatemp = 0, vtemp = 0, ctemp = 0;

        for (int i = HLWDISCARDNUM; i < HLWMAXCOLLECTCOUNT - HLWDISCARDNUM; i++) {
          wtemp += hlwvalues.ActivePower[i];
          vatemp += hlwvalues.ApparentPower[i];
          vtemp += hlwvalues.Voltage[i];
          ctemp += hlwvalues.Current[i];
        }

        wtemp =  wtemp  / (HLWMAXCOLLECTCOUNT - (HLWDISCARDNUM * 2));
        vatemp = vatemp / (HLWMAXCOLLECTCOUNT - (HLWDISCARDNUM * 2));
        vtemp =  vtemp  / (HLWMAXCOLLECTCOUNT - (HLWDISCARDNUM * 2));
        ctemp =  ctemp  / (HLWMAXCOLLECTCOUNT - (HLWDISCARDNUM * 2));

        hlw8012value.powerw = wtemp;
        hlw8012value.powerva = vatemp;
        hlw8012value.voltage = vtemp;
        hlw8012value.current = ctemp;

        if (wtemp > 0)
          hlw8012value.energy_counter += (wtemp / 3600) * (((float)HLWCOLLECTINTERVAL / 1000) * hlwvalues.HlwCollectCounter);

        hlwvalues.HlwCollectCounter = 0;
      }

      hlwvalues.ActivePower[hlwvalues.HlwCollectCounter] = hlw8012.getActivePower();
      hlwvalues.ApparentPower[hlwvalues.HlwCollectCounter] = hlw8012.getApparentPower();
      hlwvalues.Voltage[hlwvalues.HlwCollectCounter] = hlw8012.getVoltage();
      hlwvalues.Current[hlwvalues.HlwCollectCounter] = hlw8012.getCurrent();
      hlwvalues.HlwCollectCounter++;

    } else {
      hlw8012value.powerw = 0;
      hlw8012value.powerva = 0;
      hlw8012value.voltage = 0;
      hlw8012value.current = 0;
    }
  }

  if (!OTAStart && GlobalConfig.MeasureInterval > 0 &&  (millis() - LastHlwMeasureMillis) > (GlobalConfig.MeasureInterval * 1000)) {
    LastHlwMeasureMillis = millis();
    DEBUG("[HLW]: " + String(hlw8012value.powerw) + "W, " + String(hlw8012value.voltage) + "V, " + String(hlw8012value.current) + "A, " + String(hlw8012value.powerva) + "VA, Power Factor (%): " + String((int) (100 * hlw8012.getPowerFactor())) + ", ENERGY_COUNTER: " + String(hlw8012value.energy_counter) + "Wh");

    if (GlobalConfig.BackendType == BackendType_HomeMatic) {
      if (String(HomeMaticConfig.PowerVariableName) != "")
        setStateCUxD(String(HomeMaticConfig.PowerVariableName), String(hlw8012value.powerw));
      if (String(HomeMaticConfig.EnergyCounterVariableName) != "" && HomeMaticConfig.EnergyCounterVariableAvailable)
        setStateCUxD(String(HomeMaticConfig.EnergyCounterVariableName), String(hlw8012value.energy_counter));
    }
  }
}

bool getEnergyCounterValueFromCCU(bool setValue) {
  if (String(HomeMaticConfig.EnergyCounterVariableName) != "") {
    String sVal = getStateCUxD(String(HomeMaticConfig.EnergyCounterVariableName), "State");
    if (sVal == "null") {
      DEBUG("HomeMaticConfig.EnergyCounterVariableAvailable = false");
      return false;
    } else {
      DEBUG("HomeMaticConfig.EnergyCounterVariableAvailable = true");
      if (setValue) {
        hlw8012value.energy_counter = sVal.toFloat();
        DEBUG("-got value from ccu ("+String(sVal.toFloat())+")");
      } else {
        DEBUG("-not getting value from ccu");
      }
      return true;
    }
  } else {
    DEBUG("HomeMaticConfig.EnergyCounterVariableAvailable = false");
    return false;
  }
}

void sort(float *number, int n) {
  float temp = 0;
  int j, i;
  for (i = 1; i < n; i++) {
    for (j = 0; j < n - i; j++) {
      if (number[j] > number[j + 1]) {
        temp = number[j];
        number[j] = number[j + 1];
        number[j + 1] = temp;
      }
    }
  }
}
