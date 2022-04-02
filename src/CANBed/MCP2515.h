#ifndef _CANBED_MCP2515_H_
#define _CANBED_MCP2515_H_
#pragma message("Including MCP2515 CAN Controller Support")

// This controller requires the Longan MCP2515 library to build:
// https://github.com/Longan-Labs/Aruino_CAN_BUS_MCP2515

#include <mcp_can.h>
#include "controller.h"

namespace CANBed {

// CAN implementation for MCP2515 controller.
class MCP2515 : public Controller {
    public:
        // Construct a new MCP2515 CAN object that uses the given CS pin.
        MCP2515(uint8_t cs_pin) : mcp_(cs_pin), ready_(false) {}
        ~MCP2515() override = default;

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

#include "MCP2515.tpp"

#endif  // _CANBED_MCP2515_H_
