#include <dummy.h>  
#include <HardwareSerial.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <Wire.h>
#include "si7020.h"
#include "config.h"

#ifndef CONFIG_H_
  #include <example_config.h>
  /**
   * exemple
   * config.h
   * // This EUI must be in little-endian format, so least-significant-byte
   * // first. When copying an EUI from ttnctl output, this means to reverse
   * // the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3, 0x70.
   * static const u1_t PROGMEM APPEUI[8] = { 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0x70 };
   * 
   * // This should also be in little endian format, see above.
   * static const u1_t PROGMEM DEVEUI[8] = { 0x5E, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3 };
   * 
   * // This key should be in big endian format (or, since it is not really a
   * // number but a block of memory, endianness does not really apply). In
   * // practice, a key taken from ttnctl can be copied as-is.
   * 
   * static const u1_t PROGMEM APPKEY[16] = { 0x48, 0x10, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xCA, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0x62, 0x4A };
   * 
   */
#endif

#ifndef   CFG_eu868
  #error Flag CFG_eu868 not define!!!!
#endif


const unsigned TX_INTERVAL = 10;

const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {26, 33, 32},
};

char temp[10];
char hum[10];

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
Si7020 sensor;
static osjob_t sendjob;

void send_task(osjob_t* j) {
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
    u8x8.drawString(0, 7, "OP_TXRXPEND, not sent");
  } else {
    sprintf(temp, "temp : %.2f", sensor.getTemp());
    u8x8.drawString(0, 2, temp);
    Serial.println(F(temp));
    sprintf(hum, "RH : %.2f", sensor.getRH());
    u8x8.drawString(0, 3, hum);
    Serial.println(F(hum));

    LMIC_setTxData2(1, (xref2u1_t)temp, 10, 0);
 
    Serial.println(F("Packet queued"));
    u8x8.drawString(0, 7, "PACKET QUEUED");
    digitalWrite(BUILTIN_LED, HIGH);
  }
}

void onEvent (ev_t ev) {    
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      u8x8.drawString(0, 7, "EV_SCAN_TIMEOUT");
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      u8x8.drawString(0, 7, "EV_BEACON_FOUND");
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      u8x8.drawString(0, 7, "EV_BEACON_MISSED");
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      u8x8.drawString(0, 7, "EV_BEACON_TRACKED");
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      u8x8.drawString(0, 7, "EV_JOINING   ");
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      u8x8.drawString(0, 7, "EV_JOINED    ");
      // Disable link check validation (automatically enabled
      // during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      u8x8.drawString(0, 7, "EV_RFUI");
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      u8x8.drawString(0, 7, "EV_JOIN_FAILED");
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      u8x8.drawString(0, 7, "EV_REJOIN_FAILED");
      //break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      u8x8.drawString(0, 7, "EV_TXCOMPLETE");
      digitalWrite(BUILTIN_LED, LOW);
      if (LMIC.txrxFlags & TXRX_ACK) {
        Serial.println(F("Received ack"));
        u8x8.drawString(0, 7, "Received ACK");
      }
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        u8x8.drawString(0, 6, "RX ");
        Serial.println(LMIC.dataLen);
        u8x8.setCursor(4, 6);
        u8x8.printf("%i bytes", LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
        u8x8.setCursor(0, 7);
        u8x8.printf("RSSI %d SNR %.1d", LMIC.rssi, LMIC.snr);
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), send_task);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      u8x8.drawString(0, 7, "EV_LOST_TSYNC");
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      u8x8.drawString(0, 7, "EV_RESET");
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      u8x8.drawString(0, 7, "EV_RXCOMPLETE");
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      u8x8.drawString(0, 7, "EV_LINK_DEAD");
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      u8x8.drawString(0, 7, "EV_LINK_ALIVE");
      break;
    default:
      Serial.println(F("Unknown event"));
      u8x8.setCursor(0, 7);
      u8x8.printf("UNKNOWN EVENT %d", ev);
      break;
  }
}

void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}

void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

void setup() {
  Wire.begin(21,22); 
  delay(100);
  u8x8.begin();
  u8x8.setFont(u8x8_font_5x8_f);
  u8x8.drawString(0, 1,   "LoRa Configured");

  Serial.begin(9600);
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, "ADNT RBR_CARD");
  os_init();
  LMIC_reset();
  send_task(&sendjob);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

}

void loop() {
  os_runloop_once();
}