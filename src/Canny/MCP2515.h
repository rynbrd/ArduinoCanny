#ifndef _CANNY_MCP2515_H_
#define _CANNY_MCP2515_H_

// This controller requires the Longan MCP2515 library to build:
// https://github.com/Longan-Labs/Aruino_CAN_BUS_MCP2515

#include <mcp_can.h>
#include "Controller.h"

namespace Canny {

// CAN implementation for MCP2515 controller.
template <typename FrameType>
class MCP2515 : public Controller<FrameType> {
    public:
        // Construct a new MCP2515 CAN object that uses the given CS pin.
        MCP2515(uint8_t cs_pin) : mcp_(cs_pin), ready_(false) {}
        ~MCP2515() override = default;

        bool begin(Bitrate bitrate) override;
        Mode mode() const override;
        Bitrate bitrate() const override;
        Error read(FrameType* frame) override;
        Error write(const FrameType& frame) override;

        // Set a mask on the controller. The MCP2515 has two masks. Mask 0
        // applies to filters 0-1. Mask 1 applies to filters 2-5. Filtering is
        // enabled if any mask is set.
        void setMask(uint8_t num, uint8_t ext, uint32_t mask);

        // Set a filter on the controller. The MCP2515 has six filters assigned
        // to the two masks. See setMask for the mask/filter assignment.
        void setFilter(uint8_t num, uint8_t ext, uint32_t filter);

        // Clear masks and filters so that all frames are read.
        void disableFilters();
    private:
        MCP_CAN mcp_;
        bool ready_;
        Bitrate bitrate_;
};

}  // namespace Canny

#include "MCP2515.tpp"

#endif  // _CANNY_MCP2515_H_
