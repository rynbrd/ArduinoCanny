#ifndef _CANBED_DEFINITIONS_H_
#define _CANBED_DEFINITIONS_H_

#include <Arduino.h>

// Guess the CANBed board based on the microcontroller in use. It is assumed
// that this library is only used for actual CANBed devices. It doesn't appear
// as if there's a way to directly detect whether or not a Longan board is in
// use vs, for example, a regular Arduino Leonardo or P2040.

// CANBed V1
#if defined(ARDUINO_AVR_LEONARDO)
#define CANBED_V1

// CANBed M4
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
#define CANBED_M4

// CANBed RP2040 and Dual.
#elif defined(RASPBERRYPI_PICO)
#define CANBED_RP2040
#endif

#endif  // _CANBED_DEFINITIONS_H_
