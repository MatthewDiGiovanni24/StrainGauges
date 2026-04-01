#include <Wire.h>
#include <SPI.h>
#include <mcp2515.h>
#include <Adafruit_ADS1X15.h> 

struct can_frame rlsus12;
struct can_frame rlsus34;
struct can_frame rlsus5msg;
struct can_frame fem12;
struct can_frame rw12;
struct can_frame rw34;
struct can_frame rwm12;
struct can_frame rwm34;

MCP2515 mcp2515(5); 
Adafruit_ADS1115 ads1115;

struct StrainGauge {
  int32_t zero;        // zero offset
  int32_t counts;      // raw counts
  uint8_t adcChannel;  // starting channel pair (0 = diff 0/1)
};

StrainGauge rlsus1 = {0,0,0};
StrainGauge rlsus2 = {0,0,0};
StrainGauge rlsus3 = {0,0,0};
StrainGauge rlsus4 = {0,0,0};
StrainGauge rlsus5 = {0,0,0};
StrainGauge fem1 = {0,0,0};
StrainGauge fem2 = {0,0,0};
StrainGauge rw1  = {0,0,0};
StrainGauge rw2  = {0,0,0};
StrainGauge rw3  = {0,0,0};
StrainGauge rw4  = {0,0,0};
StrainGauge rwm1 = {0,0,0};
StrainGauge rwm2 = {0,0,0};
StrainGauge rwm3 = {0,0,0};
StrainGauge rwm4 = {0,0,0};


void setup() {
  Serial.begin(9600);
  Wire.begin();
  SPI.begin();
  if (!ads1115.begin()) {
    Serial.println("Failed to initialize ADS1115");
    while(1);
  }

  Serial.println("Starting MCP2515...");

  mcp2515.reset();
  if (mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ) == MCP2515::ERROR_OK) {
    Serial.println("Bitrate OK (8MHz)");
  } else {
    Serial.println("Bitrate FAILED (8MHz)");
  }

  mcp2515.setNormalMode();

  rlsus12.can_id = 100; rlsus12.can_dlc = 8;
  rlsus34.can_id = 101; rlsus34.can_dlc = 8;
  rlsus5msg.can_id  = 102; rlsus5msg.can_dlc  = 8;
  fem12.can_id   = 103; fem12.can_dlc   = 8;
  rw12.can_id    = 104; rw12.can_dlc    = 8;
  rw34.can_id    = 105; rw34.can_dlc    = 8;
  rwm12.can_id   = 106; rwm12.can_dlc   = 8;
  rwm34.can_id   = 107; rwm34.can_dlc   = 8;

  zeroGauge(rlsus1); zeroGauge(rlsus2);
  zeroGauge(rlsus3); zeroGauge(rlsus4);
  zeroGauge(rlsus5);
  zeroGauge(fem1); zeroGauge(fem2); 
  zeroGauge(rw1); zeroGauge(rw2); 
  zeroGauge(rw3); zeroGauge(rw4); 
  zeroGauge(rwm1); zeroGauge(rwm2);
  zeroGauge(rwm3); zeroGauge(rwm4); 

}

int16_t readDifferential(int channelBase) {
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

void loop() {
 // Read all gauges
  readGauge(rlsus1); readGauge(rlsus2);
  readGauge(rlsus3); readGauge(rlsus4);
  readGauge(rlsus5);
  readGauge(fem1); readGauge(fem2);
  readGauge(rw1); readGauge(rw2); readGauge(rw3); readGauge(rw4);
  readGauge(rwm1); readGauge(rwm2); readGauge(rwm3); readGauge(rwm4);

  // Store counts
  int32_t rls1count = rlsus1.counts;
  int32_t rls2count = rlsus2.counts;
  int32_t rls3count = rlsus3.counts;
  int32_t rls4count = rlsus4.counts;
  int32_t rls5count = rlsus5.counts;
  int32_t fem1count = fem1.counts;
  int32_t fem2count = fem2.counts;
  int32_t rw1count  = rw1.counts;
  int32_t rw2count  = rw2.counts;
  int32_t rw3count  = rw3.counts;
  int32_t rw4count  = rw4.counts;
  int32_t rwm1count = rwm1.counts;
  int32_t rwm2count = rwm2.counts;
  int32_t rwm3count = rwm3.counts;
  int32_t rwm4count = rwm4.counts;

  // Pack and send
  packAndSend(rlsus12, rls1count, rls2count);
  packAndSend(rlsus34, rls3count, rls4count);
  packAndSend(rlsus5msg, rls5count, 0);
  packAndSend(fem12, fem1count, fem2count);
  packAndSend(rw12, rw1count, rw2count);
  packAndSend(rw34, rw3count, rw4count);
  packAndSend(rwm12, rwm1count, rwm2count);
  packAndSend(rwm34, rwm3count, rwm4count);
}
