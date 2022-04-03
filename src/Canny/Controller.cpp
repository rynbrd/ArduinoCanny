#include "Controller.h"
#include "Frame.h"

namespace Canny {

Error Controller::read(Frame* frame) {
    if ((mode() == CAN20 && frame->Capacity() < 8) ||
            ((mode() == CANFD_CONST_RATE || mode() == CANFD_DUAL_RATE) && frame->Capacity() < 64)) {
        return ERR_INVALID;
    }
    return read(&frame->id, &frame->ext, frame->data, &frame->size);
}

Error Controller::write(const Frame& frame) {
    return write(frame.id, frame.ext, frame.data, frame.size);
}

}  // namespace Canny
