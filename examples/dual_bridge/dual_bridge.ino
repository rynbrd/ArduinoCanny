#include <Arduino.h>
#include <CANBed.h>
#include <CANBed/Detect.h>

using namespace CANBed;

uint32_t id;
uint8_t data[8];
uint8_t size;
uint8_t ext;

void setup() {
    if (!CAN.begin(CAN20_250K)) {
        Serial.println("init can failed");
        while (true) { delay(1000); }
    }
    if (!CAN1.begin(CAN20_250K)) {
        Serial.println("init can1 failed");
        while (true) { delay(1000); }
    }
}

void loop() {
    while (CAN.read(&id, &ext, data, &size) == ERR_OK) {
        if (CAN1.write(id, ext, data, size) == ERR_OK) {
            Serial.print("forward frame ");
            Serial.print(id, HEX);
            Serial.println();
        } else {
            Serial.print("failed to forward frame ");
            Serial.print(id, HEX);
            Serial.println();
        }
    }
}
