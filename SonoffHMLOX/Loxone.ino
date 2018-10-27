bool sendLoxoneUDP(String msg) {
  if (atoi(LoxoneConfig.UDPPort) > 0 && String(GlobalConfig.ccuIP).length() > 6) {
    DEBUG("Sending UDP Message = \"" + String(msg) + "\" to " + String(GlobalConfig.ccuIP) + ":" + String(LoxoneConfig.UDPPort)+", TimeStamp(millis) = "+String(millis()));
    UDPClient.UDP.beginPacket(GlobalConfig.ccuIP, atoi(LoxoneConfig.UDPPort));
    UDPClient.UDP.write(msg.c_str());
    UDPClient.UDP.endPacket();
    return true;
  } else {
    DEBUG("sendLoxoneUDP: not sending, please check MiniServer IP and UDP Port in configuration");
  }
  return false;
}
