#ifndef _CANBED_MCP2515_H_
#define _CANBED_MCP2515_H_

// Requires https://github.com/Longan-Labs/Aruino_CAN_BUS_MCP2515

#include "boards.h"
#include "can.h"

#ifdef CAN_CTRL_MCP2515
#include <mcp_can.h>

namespace CANBed {

// CAN implementation for MCP2515 controller.
class CAN2515 : public CAN {
    public:
        // Construct a new MCP2515 CAN object that uses the given CS pin.
        CAN2515(uint8_t cs_pin) : mcp_(cs_pin), ready_(false) {}
        ~CAN2515() override = default;

        bool begin(Bitrate bitrate) override;
        Mode mode() const override;
        Bitrate bitrate() const override;
        Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) override;
        Error write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) override;
    private:
        MCP_CAN mcp_;
        bool ready_;
        Bitrate bitrate_;
};

}  // namespace CANBed

#endif  // CAN_CTRL_MCP2515
#endif  // _CANBED_MCP2515_H_
