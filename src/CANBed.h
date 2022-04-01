#ifndef _CANBED_H_
#define _CANBED_H_

#include "CANBed/boards.h"
#include "CANBed/controller.h"
#include "CANBed/mcp2515.h"
#include "CANBed/mcp2518.h"
#include "CANBed/same51.h"

namespace CANBed {

#if defined(CANBED_V1)
extern MCP2515 can_mcp2515;
#define CAN ::CANBed::can_mcp2515

#elif defined(CANBED_M4)
extern SAME51 can_same51;
#define CAN ::CANBed::can_same51

#elif defined(CANBED_RP2040)
extern MCP2518 can_mcp2518;
extern MCP2515 can_mcp2515;
#define CAN ::CANBed::can_mcp2518
#define CAN1 ::CANBed::can_mcp2515

#endif

}

#endif  // _CANBED_H_
