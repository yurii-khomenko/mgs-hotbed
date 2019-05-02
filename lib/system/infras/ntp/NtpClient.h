#ifndef NtpClient_h
#define NtpClient_h

#include <Arduino.h>
#include <Udp.h>

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337

class NtpClient {
public:
  NtpClient(UDP &udp) {
    this->udpClient = &udp;
  }

  NtpClient(UDP &udp, long timeOffset) {
    this->udpClient = &udp;
    this->timeOffset = timeOffset;
  }

  NtpClient(UDP &udp, const char *poolServerName) {
    this->udpClient = &udp;
    this->host = poolServerName;
  }

  NtpClient(UDP &udp, const char *poolServerName, long timeOffset) {
    this->udpClient = &udp;
    this->timeOffset = timeOffset;
    this->host = poolServerName;
  }

  NtpClient(UDP &udp, const char *poolServerName, long timeOffset, unsigned long updateInterval) {
    this->udpClient = &udp;
    this->timeOffset = timeOffset;
    this->host = poolServerName;
    this->updateInterval = updateInterval;
  }

  void setHost(const char *host) {
    this->host = host;
  }

  /**
   * Starts the underlying UDP client with the default local port
   */
  void begin() {
    this->begin(NTP_DEFAULT_LOCAL_PORT);
  }

  /**
   * Starts the underlying UDP client with the specified local port
   */
  void begin(int port) {
    this->port = port;
    this->udpClient->begin(this->port);
    this->udpClientInit = true;
  }

  /**
   * This should be called in the main loop of your application. By default an update from the NTP Server is only
   * made every 60 seconds. This can be configured in the NtpClient constructor.
   *
   * @return true on success, false on failure
   */
  bool update() {
    if ((millis() - this->lastUpdate >= this->updateInterval)     // Update after updateInterval
        || this->lastUpdate == 0) {                                // Update if there was no update yet.
      if (!this->udpClientInit) this->begin();                         // setLevel the UDP client if needed
      return this->forceUpdate();
    }
    return true;
  }

  /**
   * This will force the update from the NTP Server.
   *
   * @return true on success, false on failure
   */
  bool forceUpdate() {

    this->sendNTPPacket();

    // Wait till data is there or timeout...
    byte timeout = 0;
    int cb = 0;
    do {
      delay(10);
      cb = this->udpClient->parsePacket();
      if (timeout > 100) return false; // timeout after 1000 ms
      timeout++;
    } while (cb == 0);

    this->lastUpdate = millis() - (10 * (timeout + 1)); // Account for delay in reading the time

    this->udpClient->read(this->packetBuffer, NTP_PACKET_SIZE);

    u64 highWord = word(this->packetBuffer[40], this->packetBuffer[41]);
    u64 lowWord = word(this->packetBuffer[42], this->packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    this->currentEpoc = secsSince1900 - SEVENZYYEARS;

    return true;
  }

  u8 getDay() {
    return (((this->getEpochTime() / 86400L) + 4) % 7); //0 is Sunday
  }

  u8 getHours() {
    return ((this->getEpochTime() % 86400L) / 3600);
  }

  u8 getMinutes() {
    return ((this->getEpochTime() % 3600) / 60);
  }

  u8 getSeconds() {
    return (this->getEpochTime() % 60);
  }

  /**
   * Changes the time offset. Useful for changing timezones dynamically
   */
  void setTimeOffset(int timeOffset) {
    this->timeOffset = timeOffset;
  }

  /**
   * Set the update interval to another frequency. E.g. useful when the
   * timeOffset should not be set in the constructor
   */
  void setUpdateInterval(unsigned long updateInterval) {
    this->updateInterval = updateInterval;
  }

  /**
   * @return time formatted like `hh:mm:ss`
   */
  String getFormattedTime() const {
    unsigned long rawTime = this->getEpochTime();
    unsigned long hours = (rawTime % 86400L) / 3600;
    String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

    unsigned long minutes = (rawTime % 3600) / 60;
    String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

    unsigned long seconds = rawTime % 60;
    String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

    return hoursStr + ":" + minuteStr + ":" + secondStr;
  }

  /**
   * @return time in seconds since Jan. 1, 1970
   */
  u64 getEpochTime() const {
    return this->timeOffset + // User offset
           this->currentEpoc + // Epoc returned by the NTP server
           ((millis() - this->lastUpdate) / 1000); // Time since last update
  }

  /**
   * Stops the underlying UDP client
   */
  void end() {
    this->udpClient->stop();
    this->udpClientInit = false;
  }

private:
  UDP *udpClient;
  bool udpClientInit = false;

  const char *host = "pool.ntp.org";
  int port = NTP_DEFAULT_LOCAL_PORT;

  u64 updateInterval = 60000;  // In ms

  s32 timeOffset = 0;

  u64 currentEpoc = 0;      // In s
  u64 lastUpdate = 0;      // In ms

  byte packetBuffer[NTP_PACKET_SIZE];

  void sendNTPPacket() {
    // set all bytes in the buffer to 0
    memset(this->packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    this->packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    this->packetBuffer[1] = 0;     // Stratum, or type of clock
    this->packetBuffer[2] = 6;     // Polling Interval
    this->packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    this->packetBuffer[12] = 49;
    this->packetBuffer[13] = 0x4E;
    this->packetBuffer[14] = 49;
    this->packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    this->udpClient->beginPacket(this->host, 123); //NTP requests are to port 123
    this->udpClient->write(this->packetBuffer, NTP_PACKET_SIZE);
    this->udpClient->endPacket();
  }
};

#endif