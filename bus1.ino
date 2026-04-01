#include <Wire.h>
#include <SPI.h>
#include <mcp2515.h>
#include <Adafruit_ADS1X15.h> 

struct can_frame flsus12;
struct can_frame flsus34;
struct can_frame flsus56;
struct can_frame flsus7msg;
struct can_frame lsi12;
struct can_frame usfp12;
struct can_frame frh12;

MCP2515 mcp2515(5); 

Adafruit_ADS1115 ads1115;

struct StrainGauge {
  int32_t zero;        // zero offset
  int32_t counts;      // raw counts
  uint8_t adcChannel;  // starting channel pair (0 = diff 0/1)
};

StrainGauge flsus1 = {0, 0, 0};
StrainGauge flsus2 = {0, 0, 0};
StrainGauge flsus3 = {0, 0, 0};
StrainGauge flsus4 = {0, 0, 0};
StrainGauge flsus5 = {0, 0, 0};
StrainGauge flsus6 = {0, 0, 0};
StrainGauge flsus7 = {0, 0, 0};
StrainGauge lsi1 = {0, 0, 0};
StrainGauge lsi2 = {0, 0, 0};
StrainGauge usfp1 = {0, 0, 0};
StrainGauge usfp2 = {0, 0, 0};
StrainGauge frh1 = {0, 0, 0};
StrainGauge frh2 = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  SPI.begin();
  if (!ads1115.begin()) {
    Serial.println("Failed to initialize ADS1115");
    while (1);
  }
  Serial.println("Starting MCP2515...");

  mcp2515.reset();
  if (mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ) == MCP2515::ERROR_OK) {
    Serial.println("Bitrate OK (8MHz)");
  } else {
    Serial.println("Bitrate FAILED (8MHz)");
  }

  mcp2515.setNormalMode();

  flsus12.can_id  = 100; flsus12.can_dlc  = 8;
  flsus34.can_id  = 101; flsus34.can_dlc  = 8;
  flsus56.can_id  = 102; flsus56.can_dlc  = 8;
  flsus7msg.can_id   = 103; flsus7msg.can_dlc   = 8;
  lsi12.can_id    = 104; lsi12.can_dlc    = 8;
  usfp12.can_id   = 105; usfp12.can_dlc   = 8;
  frh12.can_id    = 106; frh12.can_dlc    = 8;

  zeroGauge(flsus1);
  zeroGauge(flsus2);
  zeroGauge(flsus3);
  zeroGauge(flsus4);
  zeroGauge(flsus5);
  zeroGauge(flsus6);
  zeroGauge(flsus7);
  zeroGauge(lsi1);
  zeroGauge(lsi2);
  zeroGauge(usfp1);
  zeroGauge(usfp2);
  zeroGauge(frh1);
  zeroGauge(frh2);

}

int16_t readDifferential(int channelBase) {
  // channelBase = 0 → read 0/1
  // channelBase = 2 → read 2/3
  if (channelBase == 0) return ads1115.readADC_Differential_0_1();
  if (channelBase == 2) return ads1115.readADC_Differential_2_3();
  return 0; 
}

void zeroGauge(StrainGauge &g) {
  g.zero = (int32_t)readDifferential(g.adcChannel);
}

void readGauge(StrainGauge &g) {
  g.counts = (int32_t)readDifferential(g.adcChannel) - g.zero;
}

void packAndSend(struct can_frame &frame, int32_t a, int32_t b) {
  frame.data[0] = a & 0xFF;
  frame.data[1] = (a >> 8) & 0xFF;
  frame.data[2] = (a >> 16) & 0xFF;
  frame.data[3] = (a >> 24) & 0xFF;

  frame.data[4] = b & 0xFF;
  frame.data[5] = (b >> 8) & 0xFF;
  frame.data[6] = (b >> 16) & 0xFF;
  frame.data[7] = (b >> 24) & 0xFF;

  mcp2515.sendMessage(&frame);
}

void packAndSendSingle(struct can_frame &frame, int32_t a) {
  frame.data[0] = a & 0xFF;
  frame.data[1] = (a >> 8) & 0xFF;
  frame.data[2] = (a >> 16) & 0xFF;
  frame.data[3] = (a >> 24) & 0xFF;

  for (int i = 4; i < 8; i++) frame.data[i] = 0;

  
  if (mcp2515.sendMessage(&frame) == MCP2515::ERROR_OK) {
    Serial.println("Sending");
  } else {
    Serial.println("Send failed");
  }

}
void loop() {
  readGauge(flsus1); readGauge(flsus2);
  readGauge(flsus3); readGauge(flsus4);
  readGauge(flsus5); readGauge(flsus6);
  readGauge(flsus7);
  readGauge(lsi1); readGauge(lsi2);
  readGauge(usfp1); readGauge(usfp2);
  readGauge(frh1); readGauge(frh2);

  int32_t flsus1count = flsus1.counts;
  int32_t flsus2count = flsus2.counts;
  int32_t flsus3count = flsus3.counts;
  int32_t flsus4count = flsus4.counts;
  int32_t flsus5count = flsus5.counts;
  int32_t flsus6count = flsus6.counts;
  int32_t flsus7count = flsus7.counts;
  int32_t lsi1count   = lsi1.counts;
  int32_t lsi2count   = lsi2.counts;
  int32_t usfp1count  = usfp1.counts;
  int32_t usfp2count  = usfp2.counts;
  int32_t frh1count   = frh1.counts;
  int32_t frh2count   = frh2.counts;

  packAndSend(flsus12, flsus1count, flsus2count);
  packAndSend(flsus34, flsus3count, flsus4count);
  packAndSend(flsus56, flsus5count, flsus6count);
  packAndSend(flsus7msg, flsus7count, 0);

  packAndSend(lsi12, lsi1count, lsi2count);
  packAndSend(usfp12, usfp1count, usfp2count);
  packAndSend(frh12, frh1count, frh2count);

}