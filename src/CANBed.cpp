#include "CANBed.h"

#include "CANBed/boards.h"
#include "CANBed/mcp2515.h"
#include "CANBed/mcp2518.h"
#include "CANBed/same51.h"

namespace CANBed {

#if defined(CANBED_V1)
CAN2515 can(17);

#elif defined(CANBED_M4)
CANSAME51 can;

#elif defined(CANBED_RP2040)
CAN2518 can(12);
CAN2515 can1(9);

#endif

}
