#ifndef _CANBED_H_
#define _CANBED_H_

#include "CANBed/boards.h"
#include "CANBed/can.h"
#include "CANBed/mcp2515.h"
#include "CANBed/mcp2518.h"
#include "CANBed/same51.h"

namespace CANBed {

#if defined(CANBED_V1)
extern CAN2515 can;
#define CAN CANBed::can

#elif defined(CANBED_M4)
extern CANSAME51 can;
#define CAN CANBed::can

#elif defined(CANBED_RP2040)
extern CAN2518 can;
extern CAN2515 can1;
#define CAN CANBed::can
#define CAN1 CANBed::can1

#endif

}

#endif  // _CANBED_H_
