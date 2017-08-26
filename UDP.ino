String handleUDP() {
  int packetSize = UDPClient.UDP.parsePacket();
  if (packetSize) {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, UDPClient.UDP.remoteIP().toString().c_str(), UDPClient.UDP.remotePort());
    int len = UDPClient.UDP.read(UDPClient.incomingPacket, 255);
    if (len > 0)
      UDPClient.incomingPacket[len] = 0;

    Serial.printf("UDP packet contents: %s\n", UDPClient.incomingPacket);

    UDPClient.UDP.beginPacket(UDPClient.UDP.remoteIP(), UDPClient.UDP.remotePort());
    String replyMsg = createReplyString();
    UDPClient.UDP.write(replyMsg.c_str());
    UDPClient.UDP.endPacket();

    String message = String(UDPClient.incomingPacket);
    message.trim();
    return message;
  } else return "";
}
