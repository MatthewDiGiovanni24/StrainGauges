// #include <SPI.h>
// #include <mcp2515.h>

// struct can_frame canMsg;
// MCP2515 mcp2515(5); // CS pin

// #define CAN_CS   5
// #define CAN_SCK  13
// #define CAN_MOSI 23
// #define CAN_MISO 19

// void setup() {
//   Serial.begin(9600);

//   SPI.begin();

//   Serial.println("Starting MCP2515...");

//   mcp2515.reset();
//   if (mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ) == MCP2515::ERROR_OK) {
//     Serial.println("Bitrate OK (8MHz)");
//   } else {
//     Serial.println("Bitrate FAILED (8MHz)");
//   }

//   mcp2515.setNormalMode();
// }

// void loop() {
//   int tempInt = 9;

//   canMsg.can_id  = 0x036;
//   canMsg.can_dlc = 4;
//   canMsg.data[0] = (tempInt >> 8) & 0xFF;
//   canMsg.data[1] = tempInt & 0xFF;
//   canMsg.data[2] = tempInt & 0xFF;
//   canMsg.data[3] = tempInt & 0xFF;

//   if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
//     Serial.print("Sent: ");
//     Serial.println(tempInt);
//   } else {
//     Serial.println("Send failed");
//   }
// 
  // delay(1000);
// }
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(5); // CS pin

void setup() {
  Serial.begin(9600);

  SPI.begin();

  Serial.println("Starting MCP2515...");

  mcp2515.reset();
  if (mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ) == MCP2515::ERROR_OK) {
    Serial.println("Bitrate OK (8MHz)");
  } else {
    Serial.println("Bitrate FAILED (8MHz)");
  }

  mcp2515.setNormalMode();
  // mcp2515.setLoopbackMode();
}

void loop() {
 uint32_t tempUInt32 = 5700; // example 32-bit unsigned integer

canMsg.can_id = 0x064;
canMsg.can_dlc = 8; // match DBC

// Little-endian (Intel) 32-bit split
canMsg.data[0] = tempUInt32 & 0xFF;
canMsg.data[1] = (tempUInt32 >> 8) & 0xFF;
canMsg.data[2] = (tempUInt32 >> 16) & 0xFF;
canMsg.data[3] = (tempUInt32 >> 24) & 0xFF;

// remaining 4 bytes can be zero
canMsg.data[4] = 0;
canMsg.data[5] = 0;
canMsg.data[6] = 0;
canMsg.data[7] = 0;

  if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print("Sent: ");
    Serial.println(tempUInt32);
  } else {
    Serial.println("Send failed");
  }

  delay(1000);
}
// #include <SPI.h>
// #include <mcp2515.h>

// struct can_frame canMsg;
// MCP2515 mcp2515(5); // CS pin

// void setup() {
//   Serial.begin(9600);
//   SPI.begin();

//   Serial.println("Starting MCP2515...");

//   mcp2515.reset();
//   if (mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ) == MCP2515::ERROR_OK) {
//     Serial.println("Bitrate OK (8MHz)");
//   } else {
//     Serial.println("Bitrate FAILED (8MHz)");
//   }

//   mcp2515.setNormalMode();
//   // For testing without a real bus:
//   // mcp2515.setLoopbackMode();
// }

// void loop() {
//   static int tempInt = 5; // value to send

//   canMsg.can_id = 0x100;
//   canMsg.can_dlc = 4;
//   canMsg.data[0] = (tempInt >> 8) & 0xFF;
//   canMsg.data[1] = tempInt & 0xFF;
//   canMsg.data[2] = tempInt & 0xFF;
//   canMsg.data[3] = tempInt & 0xFF;

//   uint8_t result = mcp2515.sendMessage(&canMsg);

//   if (result == MCP2515::ERROR_OK) {
//     Serial.print("Sent: ");
//     Serial.println(tempInt);
//   } 
//   else if (result == MCP2515::ERROR_ALLTXBUSY) {
//     // Don't spam the serial monitor, just note retrying
//     static unsigned long lastPrint = 0;
//     if (millis() - lastPrint > 500) { // print once every 500ms
//       Serial.println("TX buffer busy, waiting...");
//       lastPrint = millis();
//     }
//   } 
//   else {
//     Serial.println("Send failed (unexpected error)");
//   }

//   delay(100); // attempt send every 100ms
// }