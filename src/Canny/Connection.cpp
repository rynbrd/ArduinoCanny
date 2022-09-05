#include "Connection.h"

#include <Arduino.h>
#include "Frame.h"

namespace Canny {

Error FilteredConnection::read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) {
    Error err = child_->read(id, ext, data, size);
    if (err == ERR_OK && !readFilter(*id, *ext, data, *size)) {
        return ERR_FIFO;
    }
    return err;
}

Error FilteredConnection::read(Frame* frame) {
    Error err = child_->read(frame);
    if (err == ERR_OK && !readFilter(frame->id(), frame->ext(), frame->data(), frame->size())) {
        return ERR_FIFO;
    }
    return err;
}

Error FilteredConnection::write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) {
    if (!writeFilter(id, ext, data, size)) {
        return ERR_OK;
    }
    return child_->write(id, ext, data, size);
}

Error FilteredConnection::write(const Frame& frame) {
    if (!writeFilter(frame.id(), frame.ext(), frame.data(), frame.size())) {
        return ERR_OK;
    }
    return child_->write(frame);
}

}  // namespace Canny
