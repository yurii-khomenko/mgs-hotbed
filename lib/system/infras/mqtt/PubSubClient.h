#ifndef PubSubClient_h
#define PubSubClient_h

#include <Arduino.h>
#include "IPAddress.h"
#include "Client.h"
#include "Stream.h"

#define MQTT_VERSION_3_1      3
#define MQTT_VERSION_3_1_1    4

// MQTT_VERSION : Pick the version
//#define MQTT_VERSION MQTT_VERSION_3_1
#ifndef MQTT_VERSION
#define MQTT_VERSION MQTT_VERSION_3_1_1
#endif

// MQTT_MAX_PACKET_SIZE : Maximum packet size
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 128
#endif

// MQTT_KEEPALIVE : keepAlive interval in Seconds
#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 15
#endif

// MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds
#ifndef MQTT_SOCKET_TIMEOUT
#define MQTT_SOCKET_TIMEOUT 15
#endif

// MQTT_MAX_TRANSFER_SIZE : limit how much data is passed to the network client
//  in each write call. Needed for the Arduino Wifi Shield. Leave undefined to
//  pass the entire MQTT packet in each write call.
//#define MQTT_MAX_TRANSFER_SIZE 80

// Possible values for client.state()
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5

#define MQTTCONNECT     1 << 4  // Client request to connect to Server
#define MQTTCONNACK     2 << 4  // Connect Acknowledgment
#define MQTTPUBLISH     3 << 4  // Publish message
#define MQTTPUBACK      4 << 4  // Publish Acknowledgment
#define MQTTPUBREC      5 << 4  // Publish Received (assured delivery part 1)
#define MQTTPUBREL      6 << 4  // Publish Release (assured delivery part 2)
#define MQTTPUBCOMP     7 << 4  // Publish Complete (assured delivery part 3)
#define MQTTSUBSCRIBE   8 << 4  // Client Subscribe request
#define MQTTSUBACK      9 << 4  // Subscribe Acknowledgment
#define MQTTUNSUBSCRIBE 10 << 4 // Client Unsubscribe request
#define MQTTUNSUBACK    11 << 4 // Unsubscribe Acknowledgment
#define MQTTPINGREQ     12 << 4 // PING Request
#define MQTTPINGRESP    13 << 4 // PING Response
#define MQTTDISCONNECT  14 << 4 // Client is Disconnecting
#define MQTTReserved    15 << 4 // Reserved

#define MQTTQOS0        (0 << 1)
#define MQTTQOS1        (1 << 1)
#define MQTTQOS2        (2 << 1)

// Maximum size of fixed header and variable length size header
#define MQTT_MAX_HEADER_SIZE 5

#if defined(ESP8266) || defined(ESP32)

#include <functional>

#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif

#define CHECK_STRING_LENGTH(l, s) if (l+2+strlen(s) > MQTT_MAX_PACKET_SIZE) {_client->stop();return false;}

class PubSubClient : public Print {
private:
  Client *_client;
  uint8_t buffer[MQTT_MAX_PACKET_SIZE];
  uint16_t nextMsgId;
  unsigned long lastOutActivity;
  unsigned long lastInActivity;
  bool pingOutstanding;
  MQTT_CALLBACK_SIGNATURE;

  uint16_t readPacket(uint8_t *lengthLength) {
    uint16_t len = 0;
    if (!readByte(buffer, &len)) return 0;
    bool isPublish = (buffer[0] & 0xF0) == MQTTPUBLISH;
    uint32_t multiplier = 1;
    uint16_t length = 0;
    uint8_t digit = 0;
    uint16_t skip = 0;
    uint8_t start = 0;

    do {
      if (len == 5) {
        // Invalid remaining length encoding - kill the connection
        _state = MQTT_DISCONNECTED;
        _client->stop();
        return 0;
      }
      if (!readByte(&digit)) return 0;
      buffer[len++] = digit;
      length += (digit & 127) * multiplier;
      multiplier *= 128;
    } while ((digit & 128) != 0);
    *lengthLength = len - 1;

    if (isPublish) {
      // Read in topic length to calculate bytes to skip over for Stream writing
      if (!readByte(buffer, &len)) return 0;
      if (!readByte(buffer, &len)) return 0;
      skip = (buffer[*lengthLength + 1] << 8) + buffer[*lengthLength + 2];
      start = 2;
      if (buffer[0] & MQTTQOS1) {
        // skip message id
        skip += 2;
      }
    }

    for (uint16_t i = start; i < length; i++) {
      if (!readByte(&digit)) return 0;
      if (this->stream) {
        if (isPublish && len - *lengthLength - 2 > skip) {
          this->stream->write(digit);
        }
      }
      if (len < MQTT_MAX_PACKET_SIZE) {
        buffer[len] = digit;
      }
      len++;
    }

    if (!this->stream && len > MQTT_MAX_PACKET_SIZE) {
      len = 0; // This will cause the packet to be ignored.
    }

    return len;
  }

  // reads a byte into result
  boolean readByte(uint8_t *result) {
    uint32_t previousMillis = millis();
    while (!_client->available()) {
      yield();
      uint32_t currentMillis = millis();
      if (currentMillis - previousMillis >= ((int32_t) MQTT_SOCKET_TIMEOUT * 1000)) {
        return false;
      }
    }
    *result = _client->read();
    return true;
  }

// reads a byte into result[*index] and increments index
  boolean readByte(uint8_t *result, uint16_t *index) {
    uint16_t current_index = *index;
    uint8_t *write_address = &(result[current_index]);
    if (readByte(write_address)) {
      *index = current_index + 1;
      return true;
    }
    return false;
  }

  boolean write(uint8_t header, uint8_t *buf, uint16_t length) {
    uint16_t rc;
    uint8_t hlen = buildHeader(header, buf, length);

#ifdef MQTT_MAX_TRANSFER_SIZE
    uint8_t* writeBuf = buf+(MQTT_MAX_HEADER_SIZE-hlen);
    uint16_t bytesRemaining = length+hlen;  //Match the length type
    uint8_t bytesToWrite;
    boolean result = true;
    while((bytesRemaining > 0) && result) {
        bytesToWrite = (bytesRemaining > MQTT_MAX_TRANSFER_SIZE)?MQTT_MAX_TRANSFER_SIZE:bytesRemaining;
        rc = _client->write(writeBuf,bytesToWrite);
        result = (rc == bytesToWrite);
        bytesRemaining -= rc;
        writeBuf += rc;
    }
    return result;
#else
    rc = _client->write(buf + (MQTT_MAX_HEADER_SIZE - hlen), length + hlen);
    lastOutActivity = millis();
    return (rc == hlen + length);
#endif
  }

  uint16_t writeString(const char *string, uint8_t *buf, uint16_t pos) {
    const char *idp = string;
    uint16_t i = 0;
    pos += 2;
    while (*idp) {
      buf[pos++] = *idp++;
      i++;
    }
    buf[pos - i - 2] = (i >> 8);
    buf[pos - i - 1] = (i & 0xFF);
    return pos;
  }

  // Build up the header ready to send
  // Returns the size of the header
  // Note: the header is built at the end of the first MQTT_MAX_HEADER_SIZE bytes, so will start
  //       (MQTT_MAX_HEADER_SIZE - <returned size>) bytes into the buffer
  size_t buildHeader(uint8_t header, uint8_t *buf, uint16_t length) {
    uint8_t lenBuf[4];
    uint8_t llen = 0;
    uint8_t digit;
    uint8_t pos = 0;
    uint16_t len = length;
    do {
      digit = len % 128;
      len = len / 128;
      if (len > 0) {
        digit |= 0x80;
      }
      lenBuf[pos++] = digit;
      llen++;
    } while (len > 0);

    buf[4 - llen] = header;
    for (int i = 0; i < llen; i++) {
      buf[MQTT_MAX_HEADER_SIZE - llen + i] = lenBuf[i];
    }
    return llen + 1; // Full header size is variable length bit plus the 1-byte fixed header
  }

  IPAddress ip;
  const char *domain;
  uint16_t port;
  Stream *stream;
  int _state;
public:
  PubSubClient() {
    this->_state = MQTT_DISCONNECTED;
    this->_client = NULL;
    this->stream = NULL;
    setCallback(NULL);
  }

  PubSubClient(Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(IPAddress addr, uint16_t port, Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setServer(addr, port);
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(IPAddress addr, uint16_t port, Client &client, Stream &stream) {
    this->_state = MQTT_DISCONNECTED;
    setServer(addr, port);
    setClient(client);
    setStream(stream);
  }

  PubSubClient(IPAddress addr, uint16_t port, MQTT_CALLBACK_SIGNATURE, Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setServer(addr, port);
    setCallback(callback);
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(IPAddress addr, uint16_t port, MQTT_CALLBACK_SIGNATURE, Client &client, Stream &stream) {
    this->_state = MQTT_DISCONNECTED;
    setServer(addr, port);
    setCallback(callback);
    setClient(client);
    setStream(stream);
  }

  PubSubClient(uint8_t *ip, uint16_t port, Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setServer(ip, port);
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(uint8_t *ip, uint16_t port, Client &client, Stream &stream) {
    this->_state = MQTT_DISCONNECTED;
    setServer(ip, port);
    setClient(client);
    setStream(stream);
  }

  PubSubClient(uint8_t *ip, uint16_t port, MQTT_CALLBACK_SIGNATURE, Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setServer(ip, port);
    setCallback(callback);
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(uint8_t *ip, uint16_t port, MQTT_CALLBACK_SIGNATURE, Client &client, Stream &stream) {
    this->_state = MQTT_DISCONNECTED;
    setServer(ip, port);
    setCallback(callback);
    setClient(client);
    setStream(stream);
  }

  PubSubClient(const char *domain, uint16_t port, Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setServer(domain, port);
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(const char *domain, uint16_t port, Client &client, Stream &stream) {
    this->_state = MQTT_DISCONNECTED;
    setServer(domain, port);
    setClient(client);
    setStream(stream);
  }

  PubSubClient(const char *domain, uint16_t port, MQTT_CALLBACK_SIGNATURE, Client &client) {
    this->_state = MQTT_DISCONNECTED;
    setServer(domain, port);
    setCallback(callback);
    setClient(client);
    this->stream = NULL;
  }

  PubSubClient(const char *domain, uint16_t port, MQTT_CALLBACK_SIGNATURE, Client &client, Stream &stream) {
    this->_state = MQTT_DISCONNECTED;
    setServer(domain, port);
    setCallback(callback);
    setClient(client);
    setStream(stream);
  }

  PubSubClient &setServer(uint8_t *ip, uint16_t port) {
    IPAddress addr(ip[0], ip[1], ip[2], ip[3]);
    return setServer(addr, port);
  }

  PubSubClient &setServer(IPAddress ip, uint16_t port) {
    this->ip = ip;
    this->port = port;
    this->domain = NULL;
    return *this;
  }

  PubSubClient &setServer(const char *domain, uint16_t port) {
    this->domain = domain;
    this->port = port;
    return *this;
  }

  PubSubClient &setCallback(MQTT_CALLBACK_SIGNATURE) {
    this->callback = callback;
    return *this;
  }

  PubSubClient &setClient(Client &client) {
    this->_client = &client;
    return *this;
  }

  PubSubClient &setStream(Stream &stream) {
    this->stream = &stream;
    return *this;
  }

  boolean connect(const char *id) {
    return connect(id, NULL, NULL, 0, 0, 0, 0, 1);
  }

  boolean connect(const char *id, const char *user, const char *pass) {
    return connect(id, user, pass, 0, 0, 0, 0, 1);
  }

  boolean connect(const char *id, const char *willTopic, uint8_t willQos, boolean willRetain, const char *willMessage) {
    return connect(id, NULL, NULL, willTopic, willQos, willRetain, willMessage, 1);
  }

  boolean connect(const char *id, const char *user, const char *pass, const char *willTopic, uint8_t willQos,
                  boolean willRetain, const char *willMessage) {
    return connect(id, user, pass, willTopic, willQos, willRetain, willMessage, 1);
  }

  boolean connect(const char *id, const char *user, const char *pass, const char *willTopic, uint8_t willQos,
                  boolean willRetain, const char *willMessage, boolean cleanSession) {
    if (!connected()) {
      int result = 0;

      if (domain != NULL) {
        result = _client->connect(this->domain, this->port);
      } else {
        result = _client->connect(this->ip, this->port);
      }
      if (result == 1) {
        nextMsgId = 1;
        // Leave room in the buffer for header and variable length field
        uint16_t length = MQTT_MAX_HEADER_SIZE;
        unsigned int j;

#if MQTT_VERSION == MQTT_VERSION_3_1
        uint8_t d[9] = {0x00,0x06,'M','Q','I','s','d','p', MQTT_VERSION};
#define MQTT_HEADER_VERSION_LENGTH 9
#elif MQTT_VERSION == MQTT_VERSION_3_1_1
        uint8_t d[7] = {0x00, 0x04, 'M', 'Q', 'T', 'T', MQTT_VERSION};
#define MQTT_HEADER_VERSION_LENGTH 7
#endif
        for (j = 0; j < MQTT_HEADER_VERSION_LENGTH; j++) {
          buffer[length++] = d[j];
        }

        uint8_t v;
        if (willTopic) {
          v = 0x04 | (willQos << 3) | (willRetain << 5);
        } else {
          v = 0x00;
        }
        if (cleanSession) {
          v = v | 0x02;
        }

        if (user != NULL) {
          v = v | 0x80;

          if (pass != NULL) {
            v = v | (0x80 >> 1);
          }
        }

        buffer[length++] = v;

        buffer[length++] = ((MQTT_KEEPALIVE) >> 8);
        buffer[length++] = ((MQTT_KEEPALIVE) & 0xFF);

        CHECK_STRING_LENGTH(length, id)
        length = writeString(id, buffer, length);
        if (willTopic) {
          CHECK_STRING_LENGTH(length, willTopic)
          length = writeString(willTopic, buffer, length);
          CHECK_STRING_LENGTH(length, willMessage)
          length = writeString(willMessage, buffer, length);
        }

        if (user != NULL) {
          CHECK_STRING_LENGTH(length, user)
          length = writeString(user, buffer, length);
          if (pass != NULL) {
            CHECK_STRING_LENGTH(length, pass)
            length = writeString(pass, buffer, length);
          }
        }

        write(MQTTCONNECT, buffer, length - MQTT_MAX_HEADER_SIZE);

        lastInActivity = lastOutActivity = millis();

        while (!_client->available()) {
          unsigned long t = millis();
          if (t - lastInActivity >= ((int32_t) MQTT_SOCKET_TIMEOUT * 1000UL)) {
            _state = MQTT_CONNECTION_TIMEOUT;
            _client->stop();
            return false;
          }
        }
        uint8_t llen;
        uint16_t len = readPacket(&llen);

        if (len == 4) {
          if (buffer[3] == 0) {
            lastInActivity = millis();
            pingOutstanding = false;
            _state = MQTT_CONNECTED;
            return true;
          } else {
            _state = buffer[3];
          }
        }
        _client->stop();
      } else {
        _state = MQTT_CONNECT_FAILED;
      }
      return false;
    }
    return true;
  }

  void disconnect() {
    buffer[0] = MQTTDISCONNECT;
    buffer[1] = 0;
    _client->write(buffer, 2);
    _state = MQTT_DISCONNECTED;
    _client->flush();
    _client->stop();
    lastInActivity = lastOutActivity = millis();
  }

  boolean publish(const char *topic, const char *payload) {
    return publish(topic, (const uint8_t *) payload, strlen(payload), false);
  }

  boolean publish(const char *topic, const char *payload, boolean retained) {
    return publish(topic, (const uint8_t *) payload, strlen(payload), retained);
  }

  boolean publish(const char *topic, const uint8_t *payload, unsigned int plength) {
    return publish(topic, payload, plength, false);
  }

  boolean publish(const char *topic, const uint8_t *payload, unsigned int plength, boolean retained) {
    if (connected()) {
      if (MQTT_MAX_PACKET_SIZE < MQTT_MAX_HEADER_SIZE + 2 + strlen(topic) + plength) {
        // Too long
        return false;
      }
      // Leave room in the buffer for header and variable length field
      uint16_t length = MQTT_MAX_HEADER_SIZE;
      length = writeString(topic, buffer, length);
      uint16_t i;
      for (i = 0; i < plength; i++) {
        buffer[length++] = payload[i];
      }
      uint8_t header = MQTTPUBLISH;
      if (retained) {
        header |= 1;
      }
      return write(header, buffer, length - MQTT_MAX_HEADER_SIZE);
    }
    return false;
  }

  boolean publish_P(const char *topic, const char *payload, boolean retained) {
    return publish_P(topic, (const uint8_t *) payload, strlen(payload), retained);
  }

  boolean publish_P(const char *topic, const uint8_t *payload, unsigned int plength, boolean retained) {
    uint8_t llen = 0;
    uint8_t digit;
    unsigned int rc = 0;
    uint16_t tlen;
    unsigned int pos = 0;
    unsigned int i;
    uint8_t header;
    unsigned int len;

    if (!connected()) {
      return false;
    }

    tlen = strlen(topic);

    header = MQTTPUBLISH;
    if (retained) {
      header |= 1;
    }
    buffer[pos++] = header;
    len = plength + 2 + tlen;
    do {
      digit = len % 128;
      len = len / 128;
      if (len > 0) {
        digit |= 0x80;
      }
      buffer[pos++] = digit;
      llen++;
    } while (len > 0);

    pos = writeString(topic, buffer, pos);

    rc += _client->write(buffer, pos);

    for (i = 0; i < plength; i++) {
      rc += _client->write((char) pgm_read_byte_near(payload + i));
    }

    lastOutActivity = millis();

    return rc == tlen + 4 + plength;
  }

  // Start to publish a message.
  // This API:
  //   beginPublish(...)
  //   one or more calls to write(...)
  //   endPublish()
  // Allows for arbitrarily large payloads to be sent without them having to be copied into
  // a new buffer and held in memory at one time
  // Returns 1 if the message was started successfully, 0 if there was an error
  boolean beginPublish(const char *topic, unsigned int plength, boolean retained) {
    if (connected()) {
      // Send the header and variable length field
      uint16_t length = MQTT_MAX_HEADER_SIZE;
      length = writeString(topic, buffer, length);
      uint16_t i;
      uint8_t header = MQTTPUBLISH;
      if (retained) {
        header |= 1;
      }
      size_t hlen = buildHeader(header, buffer, plength + length - MQTT_MAX_HEADER_SIZE);
      uint16_t rc = _client->write(buffer + (MQTT_MAX_HEADER_SIZE - hlen), length - (MQTT_MAX_HEADER_SIZE - hlen));
      lastOutActivity = millis();
      return (rc == (length - (MQTT_MAX_HEADER_SIZE - hlen)));
    }
    return false;
  }

  // Finish off this publish message (started with beginPublish)
  // Returns 1 if the packet was sent successfully, 0 if there was an error
  int endPublish() {
    return 1;
  }

  // Write a single byte of payload (only to be used with beginPublish/endPublish)
  size_t write(uint8_t data) {
    lastOutActivity = millis();
    return _client->write(data);
  }

  // Write size bytes from buffer into the payload (only to be used with beginPublish/endPublish)
  // Returns the number of bytes written
  size_t write(const uint8_t *buffer, size_t size) {
    lastOutActivity = millis();
    return _client->write(buffer, size);
  }

  boolean subscribe(const char *topic) {
    return subscribe(topic, 0);
  }

  boolean subscribe(const char *topic, uint8_t qos) {
    if (qos > 1) {
      return false;
    }
    if (MQTT_MAX_PACKET_SIZE < 9 + strlen(topic)) {
      // Too long
      return false;
    }
    if (connected()) {
      // Leave room in the buffer for header and variable length field
      uint16_t length = MQTT_MAX_HEADER_SIZE;
      nextMsgId++;
      if (nextMsgId == 0) {
        nextMsgId = 1;
      }
      buffer[length++] = (nextMsgId >> 8);
      buffer[length++] = (nextMsgId & 0xFF);
      length = writeString((char *) topic, buffer, length);
      buffer[length++] = qos;
      return write(MQTTSUBSCRIBE | MQTTQOS1, buffer, length - MQTT_MAX_HEADER_SIZE);
    }
    return false;
  }

  boolean unsubscribe(const char *topic) {
    if (MQTT_MAX_PACKET_SIZE < 9 + strlen(topic)) {
      // Too long
      return false;
    }
    if (connected()) {
      uint16_t length = MQTT_MAX_HEADER_SIZE;
      nextMsgId++;
      if (nextMsgId == 0) {
        nextMsgId = 1;
      }
      buffer[length++] = (nextMsgId >> 8);
      buffer[length++] = (nextMsgId & 0xFF);
      length = writeString(topic, buffer, length);
      return write(MQTTUNSUBSCRIBE | MQTTQOS1, buffer, length - MQTT_MAX_HEADER_SIZE);
    }
    return false;
  }

  boolean connected() {
    boolean rc;
    if (_client == NULL) {
      rc = false;
    } else {
      rc = (int) _client->connected();
      if (!rc) {
        if (this->_state == MQTT_CONNECTED) {
          this->_state = MQTT_CONNECTION_LOST;
          _client->flush();
          _client->stop();
        }
      }
    }
    return rc;
  }

  int state() {
    return this->_state;
  }

  boolean loop() {
    if (connected()) {
      unsigned long t = millis();
      if ((t - lastInActivity > MQTT_KEEPALIVE * 1000UL) || (t - lastOutActivity > MQTT_KEEPALIVE * 1000UL)) {
        if (pingOutstanding) {
          this->_state = MQTT_CONNECTION_TIMEOUT;
          _client->stop();
          return false;
        } else {
          buffer[0] = MQTTPINGREQ;
          buffer[1] = 0;
          _client->write(buffer, 2);
          lastOutActivity = t;
          lastInActivity = t;
          pingOutstanding = true;
        }
      }
      if (_client->available()) {
        uint8_t llen;
        uint16_t len = readPacket(&llen);
        uint16_t msgId = 0;
        uint8_t *payload;
        if (len > 0) {
          lastInActivity = t;
          uint8_t type = buffer[0] & 0xF0;
          if (type == MQTTPUBLISH) {
            if (callback) {
              uint16_t tl = (buffer[llen + 1] << 8) + buffer[llen + 2]; /* topic length in bytes */
              memmove(buffer + llen + 2, buffer + llen + 3, tl); /* move topic inside buffer 1 byte to front */
              buffer[llen + 2 + tl] = 0; /* end the topic as a 'C' string with \x00 */
              char *topic = (char *) buffer + llen + 2;
              // msgId only present for QOS>0
              if ((buffer[0] & 0x06) == MQTTQOS1) {
                msgId = (buffer[llen + 3 + tl] << 8) + buffer[llen + 3 + tl + 1];
                payload = buffer + llen + 3 + tl + 2;
                callback(topic, payload, len - llen - 3 - tl - 2);

                buffer[0] = MQTTPUBACK;
                buffer[1] = 2;
                buffer[2] = (msgId >> 8);
                buffer[3] = (msgId & 0xFF);
                _client->write(buffer, 4);
                lastOutActivity = t;

              } else {
                payload = buffer + llen + 3 + tl;
                callback(topic, payload, len - llen - 3 - tl);
              }
            }
          } else if (type == MQTTPINGREQ) {
            buffer[0] = MQTTPINGRESP;
            buffer[1] = 0;
            _client->write(buffer, 2);
          } else if (type == MQTTPINGRESP) {
            pingOutstanding = false;
          }
        } else if (!connected()) {
          // readPacket has closed the connection
          return false;
        }
      }
      return true;
    }
    return false;
  }
};


#endif
