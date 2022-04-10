#include "Controller.h"
#include "Frame.h"

namespace Canny {

Error Controller::read(Frame* frame) {
    switch (mode()) {
        case CAN20:
            frame->reserve(8);
            break;
        case CANFD_CONST_RATE:
        case CANFD_DUAL_RATE:
            frame->reserve(64);
            break;
    }
    return read(frame->mutable_id(), frame->mutable_ext(), frame->data(), frame->mutable_size());
}

Error Controller::write(const Frame& frame) {
    return write(frame.id(), frame.ext(), frame.data(), frame.size());
}

}  // namespace Canny
