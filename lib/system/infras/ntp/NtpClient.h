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
    this->_udp = &udp;
  }

  NtpClient(UDP &udp, long timeOffset) {
    this->_udp = &udp;
    this->_timeOffset = timeOffset;
  }

  NtpClient(UDP &udp, const char *poolServerName) {
    this->_udp = &udp;
    this->_poolServerName = poolServerName;
  }

  NtpClient(UDP &udp, const char *poolServerName, long timeOffset) {
    this->_udp = &udp;
    this->_timeOffset = timeOffset;
    this->_poolServerName = poolServerName;
  }

  NtpClient(UDP &udp, const char *poolServerName, long timeOffset, unsigned long updateInterval) {
    this->_udp = &udp;
    this->_timeOffset = timeOffset;
    this->_poolServerName = poolServerName;
    this->_updateInterval = updateInterval;
  }

  /**
   * Set time server name
   *
   * @param poolServerName
   */
  void setPoolServerName(const char *poolServerName) {
    this->_poolServerName = poolServerName;
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
    this->_port = port;
    this->_udp->begin(this->_port);
    this->_udpSetup = true;
  }

  /**
   * This should be called in the main loop of your application. By default an update from the NTP Server is only
   * made every 60 seconds. This can be configured in the NtpClient constructor.
   *
   * @return true on success, false on failure
   */
  bool update() {
    if ((millis() - this->_lastUpdate >= this->_updateInterval)     // Update after _updateInterval
        || this->_lastUpdate == 0) {                                // Update if there was no update yet.
      if (!this->_udpSetup) this->begin();                         // setLevel the UDP client if needed
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
      cb = this->_udp->parsePacket();
      if (timeout > 100) return false; // timeout after 1000 ms
      timeout++;
    } while (cb == 0);

    this->_lastUpdate = millis() - (10 * (timeout + 1)); // Account for delay in reading the time

    this->_udp->read(this->_packetBuffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(this->_packetBuffer[40], this->_packetBuffer[41]);
    unsigned long lowWord = word(this->_packetBuffer[42], this->_packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    this->_currentEpoc = secsSince1900 - SEVENZYYEARS;

    return true;
  }

  int getDay() {
    return (((this->getEpochTime() / 86400L) + 4) % 7); //0 is Sunday
  }

  int getHours() {
    return ((this->getEpochTime() % 86400L) / 3600);
  }

  int getMinutes() {
    return ((this->getEpochTime() % 3600) / 60);
  }

  int getSeconds() {
    return (this->getEpochTime() % 60);
  }

  /**
   * Changes the time offset. Useful for changing timezones dynamically
   */
  void setTimeOffset(int timeOffset) {
    this->_timeOffset = timeOffset;
  }

  /**
   * Set the update interval to another frequency. E.g. useful when the
   * timeOffset should not be set in the constructor
   */
  void setUpdateInterval(unsigned long updateInterval) {
    this->_updateInterval = updateInterval;
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
  unsigned long getEpochTime() const {
    return this->_timeOffset + // User offset
           this->_currentEpoc + // Epoc returned by the NTP server
           ((millis() - this->_lastUpdate) / 1000); // Time since last update
  }

  /**
   * Stops the underlying UDP client
   */
  void end() {
    this->_udp->stop();
    this->_udpSetup = false;
  }

private:
  UDP *_udp;
  bool _udpSetup = false;

  const char *_poolServerName = "pool.ntp.org"; // Default time server
  int _port = NTP_DEFAULT_LOCAL_PORT;
  long _timeOffset = 0;

  unsigned long _updateInterval = 60000;  // In ms

  unsigned long _currentEpoc = 0;      // In s
  unsigned long _lastUpdate = 0;      // In ms

  byte _packetBuffer[NTP_PACKET_SIZE];

  void sendNTPPacket() {
    // set all bytes in the buffer to 0
    memset(this->_packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    this->_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    this->_packetBuffer[1] = 0;     // Stratum, or type of clock
    this->_packetBuffer[2] = 6;     // Polling Interval
    this->_packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    this->_packetBuffer[12] = 49;
    this->_packetBuffer[13] = 0x4E;
    this->_packetBuffer[14] = 49;
    this->_packetBuffer[15] = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    this->_udp->beginPacket(this->_poolServerName, 123); //NTP requests are to port 123
    this->_udp->write(this->_packetBuffer, NTP_PACKET_SIZE);
    this->_udp->endPacket();
  }
};


#endif