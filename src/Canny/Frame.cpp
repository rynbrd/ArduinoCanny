#include "Frame.h"

#include <Arduino.h>

namespace Canny {

Frame::Frame(size_t capacity) : id(0), ext(0), size(0),
    data(new uint8_t[capacity]), capacity_(capacity), free_(true) {}

Frame::Frame(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity, bool free) :
    id(id), ext(ext), size(size), data(data), capacity_(capacity), free_(free) {}

Frame::~Frame() {
    if (free_) {
        delete data;
    } 
}

Frame Frame::Copy(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity) {
    if (size > capacity) {
        capacity = size;
    }
    uint8_t* owned_data = new uint8_t[capacity];
    memcpy(owned_data, data, size);
    if (capacity > size) {
        memset(owned_data+size, 0, capacity-size);
    }
    return Frame(id, ext, owned_data, size, capacity, true);
}

Frame Frame::Wrap(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity) {
    if (size > capacity) {
        capacity = size;
    }
    return Frame(id, ext, data, size, capacity, false);
}

uint8_t Frame::Capacity() const {
    return capacity_;
}

}  // namespace Canny
