#ifndef _CANBED_H_
#define _CANBED_H_

#include "CANBed/Boards.h"
#include "CANBed/Controller.h"


#if defined(CANBED_V1)
#include "CANBed/MCP2515.h"
namespace CANBed {
MCP2515 can_mcp2515(17);
}
#define CAN ::CANBed::can_mcp2515

#elif defined(CANBED_M4)
#include "CANBed/SAME51.h"
namespace CANBed {
SAME51 can_same51;
}
#define CAN ::CANBed::can_same51

#elif defined(CANBED_RP2040)
#include "CANBed/MCP2518.h"
#include "CANBed/MCP2515.h"
namespace CANBed {
MCP2518 can_mcp2518(12);
MCP2515 can_mcp2515(9);
}
#define CAN ::CANBed::can_mcp2518
#define CAN1 ::CANBed::can_mcp2515

#endif

#endif  // _CANBED_H_
