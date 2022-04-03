#ifndef _CANNY_MCP2518_H_
#define _CANNY_MCP2518_H_
#pragma message("Including MCP2518 CAN Controller Support")

// This requires the Longan CANFD library to build:
// https://github.com/Longan-Labs/Longan_CANFD

#include <mcp2518fd_can.h>
#include "Controller.h"

namespace Canny {

// CAN implementation for the MCP2517 and MCP2518 controllers.
class MCP2518 : public Controller {
    public:
        MCP2518(uint8_t cs_pin) : mcp_(cs_pin), ready_(false) {}
        ~MCP2518() override = default;

        bool begin(Bitrate bitrate) override;
        Mode mode() const override;
        Bitrate bitrate() const override;
        Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) override;
        Error write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) override;
    private:
        mcp2518fd mcp_;
        bool ready_;
        Mode mode_;
        Bitrate bitrate_;
};

}  // namespace Canny

#include "MCP2518.tpp"

#endif  // _CANNY_MCP2518_H_