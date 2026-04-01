// #include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
struct can_frame ackMsg;

MCP2515 mcp2515(53);   // CS pin (must match wiring)

#define TEMP_CAN_ID 0x036
#define CAN_ACK_ID  0x037

void setup() {
  Serial.begin(9600);

  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  Serial.println("CAN Receiver Ready");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print("ID: ");
    Serial.println(canMsg.can_id, HEX);

    Serial.print("Data: ");
    for (int i = 0; i < canMsg.can_dlc; i++) {
      Serial.print(canMsg.data[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}