#ifndef _CANNY_SAME51_H_
#define _CANNY_SAME51_H_

#include <same51_can.h>
#include "Controller.h"

namespace Canny {

// CAN implementation for SAME51 boards with integrated CAN FD controller.
template <typename FrameType>
class SAME51 : public Controller<FrameType> {
    public:
        // Construct a new CAN object.
        SAME51() : same51_(), ready_(false) {}
        ~SAME51() override = default;

        bool begin(Bitrate bitrate) override;
        Mode mode() const override;
        Bitrate bitrate() const override;
        Error read(FrameType* frame) override;
        Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size);
        Error write(const FrameType& frame) override;
        Error write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size);
    private:
        SAME51_CAN same51_;
        bool ready_;
        Mode mode_;
        Bitrate bitrate_;
};

}  // namespace Canny

#include "SAME51.tpp"

#endif  // _CANNY_SAME51_H_
