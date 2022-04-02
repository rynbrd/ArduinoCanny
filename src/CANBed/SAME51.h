#ifndef _CANBED_SAME51_H_
#define _CANBED_SAME51_H_
#pragma message("Including SAME51 CAN Controller Support")

#include <same51_can.h>
#include "controller.h"

namespace CANBed {

// CAN implementation for SAME51 boards with integrated CAN FD controller.
class SAME51 : public Controller {
    public:
        // Construct a new CAN object.
        SAME51() : same51_(), ready_(false) {}
        ~SAME51() override = default;

        bool begin(Bitrate bitrate) override;
        Mode mode() const override;
        Bitrate bitrate() const override;
        Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) override;
        Error write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) override;
    private:
        SAME51_CAN same51_;
        bool ready_;
        Mode mode_;
        Bitrate bitrate_;
};

}  // namespace CANBed

#include "SAME51.tpp"

#endif  // _CANBED_SAME51_H_
