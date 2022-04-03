#include "Frame.h"

#include <Arduino.h>

namespace Canny {
namespace {

inline uint8_t* CopyData(uint8_t* data, uint8_t size, uint8_t capacity) {
    if (size > capacity) {
        capacity = size;
    }
    uint8_t* copied_data = new uint8_t[capacity];
    memcpy(copied_data, data, size);
    if (capacity > size) {
        memset(copied_data+size, 0, capacity-size);
    }
    return copied_data;
}

}  // namespace

Frame::Frame(size_t capacity) : id(0), ext(0), size(0),
    data(new uint8_t[capacity]), capacity_(capacity), free_(true) {}

Frame::Frame(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity, bool free) :
    id(id), ext(ext), size(size), data(data), capacity_(capacity), free_(free) {}

Frame::Frame(const Frame& frame) : id(frame.id), ext(frame.ext), size(frame.size),
    data(frame.free_ ? CopyData(frame.data, frame.size, frame.capacity_) : frame.data),
    capacity_(frame.capacity_), free_(frame.free_) {}

Frame::~Frame() {
    if (free_) {
        delete data;
    } 
}

Frame Frame::Copy(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity) {
    return Frame(id, ext, CopyData(data, size, capacity), size, capacity, true);
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
