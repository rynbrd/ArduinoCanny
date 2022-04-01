#include "CANBed.h"

#include "CANBed/boards.h"
#include "CANBed/mcp2515.h"
#include "CANBed/mcp2518.h"
#include "CANBed/same51.h"

namespace CANBed {

#if defined(CANBED_V1)
MCP2515 can_mcp2515(17);

#elif defined(CANBED_M4)
SAME51 can_same51;

#elif defined(CANBED_RP2040)
MCP2518 can_mcp2518(12);
MCP2515 can_mcp2515(9);

#endif

}
