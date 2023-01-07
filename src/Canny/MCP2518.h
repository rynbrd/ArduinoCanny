#ifndef _CANNY_MCP2518_H_
#define _CANNY_MCP2518_H_

// This requires the Longan CANFD library to build:
// https://github.com/Longan-Labs/Longan_CANFD

#include <mcp2518fd_can.h>
#include "Controller.h"

namespace Canny {

// CAN implementation for the MCP2517 and MCP2518 controllers.
template <typename FrameType>
class MCP2518 : public Controller<FrameType> {
    public:
        MCP2518(uint8_t cs_pin) : mcp_(cs_pin), ready_(false) {}
        ~MCP2518() override = default;

        bool begin(Bitrate bitrate) override;
        Mode mode() const override;
        Bitrate bitrate() const override;
        Error read(FrameType* frame) override;
        Error write(const FrameType& frame) override;

        // Enable a read filter on the controller. The controller has 32
        // available filters. Frames whose ID does not match one of the filters
        // is discarded. Filtering is enabled if any filter is set.
        void setFilter(uint8_t num, uint8_t ext, uint32_t filter, uint32_t mask);

        // Clear a filter.
        void clearFilter(uint8_t num);

        // Clear all filters so that all frames are read.
        void disableFilters(); 
    private:
        mcp2518fd mcp_;
        bool ready_;
        Mode mode_;
        Bitrate bitrate_;
};

}  // namespace Canny

#include "MCP2518.tpp"

#endif  // _CANNY_MCP2518_H_
