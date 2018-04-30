String handleUDP() {
  int packetSize = UDPClient.UDP.parsePacket();
  if (packetSize) {
    DEBUG("Received "+String(packetSize)+" bytes from "+UDPClient.UDP.remoteIP().toString()+", port "+String(UDPClient.UDP.remotePort()));
    int len = UDPClient.UDP.read(UDPClient.incomingPacket, 255);
    if (len > 0)
      UDPClient.incomingPacket[len] = 0;

    DEBUG("UDP packet contents: " + String(UDPClient.incomingPacket));

    UDPClient.UDP.beginPacket(UDPClient.UDP.remoteIP(), UDPClient.UDP.remotePort());
    String replyMsg = createReplyString();
    UDPClient.UDP.write(replyMsg.c_str());
    UDPClient.UDP.endPacket();

    String message = String(UDPClient.incomingPacket);
    message.trim();
    return message;
  } else return "";
}
