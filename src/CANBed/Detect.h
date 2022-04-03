#ifndef _CANBED_DETECT_H_
#define _CANBED_DETECT_H_

#include <Arduino.h>

// Guess the CANBed board based on the microcontroller in use. It is assumed
// that this library is only used for actual CANBed devices. It doesn't appear
// as if there's a way to directly detect whether or not a Longan board is in
// use vs, for example, a regular Arduino Leonardo or P2040.

// CANBed V1
#if defined(ARDUINO_AVR_LEONARDO)
#include "CANBed/MCP2515.h"
namespace CANBed {
MCP2515 can_mcp2515(17);
}
#define CAN ::CANBed::can_mcp2515

// CANBed M4
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
#include "CANBed/SAME51.h"
namespace CANBed {
SAME51 can_same51;
}
#define CAN ::CANBed::can_same51

// CANBed RP2040 and Dual.
#elif defined(RASPBERRYPI_PICO)
#include "CANBed/MCP2518.h"
#include "CANBed/MCP2515.h"
namespace CANBed {
MCP2518 can_mcp2518(12);
MCP2515 can_mcp2515(9);
}
#define CAN ::CANBed::can_mcp2518
#define CAN1 ::CANBed::can_mcp2515

#endif

#endif  // _CANBED_DETECT_H_
