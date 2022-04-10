#include "Frame.h"

#include <Arduino.h>

namespace Canny {
namespace {

inline uint8_t* CopyData(uint8_t* data, uint8_t size, uint8_t capacity, uint8_t fill=0x00) {
    if (size > capacity) {
        capacity = size;
    }
    uint8_t* copied_data = new uint8_t[capacity];
    memcpy(copied_data, data, size);
    if (capacity > size) {
        memset(copied_data+size, fill, capacity-size);
    }
    return copied_data;
}

inline uint8_t* CopyData(const std::initializer_list<uint8_t>& data, uint8_t capacity, uint8_t fill=0x00) {
    if (data.size() > capacity) {
        capacity = data.size();
    }
    uint8_t* copied_data = new uint8_t[capacity];
    size_t i = 0;
    for (auto it = data.begin(); it != data.end(); it++) {
        copied_data[i++] = *it;
    }
    if (capacity > data.size()) {
        memset(copied_data+data.size(), fill, capacity-data.size());
    }
    return copied_data;
}

}  // namespace

Frame::Frame() : id(0), ext(0), size(0), data(nullptr), capacity_(0), free_(false) {}

Frame::Frame(uint8_t capacity, uint8_t fill) : id(0), ext(0), size(0),
    data(new uint8_t[capacity]), capacity_(capacity), free_(true) {
    Clear(fill);
}

Frame::Frame(uint32_t id, uint8_t ext, uint8_t size, uint8_t capacity, uint8_t fill) :
    id(id), ext(ext), size(size),
    data(new uint8_t[capacity > size ? capacity : size]),
    capacity_(capacity > size ? capacity : size), free_(true) {
    Clear(fill);
}

Frame::Frame(uint32_t id, uint8_t ext, std::initializer_list<uint8_t> data, uint8_t capacity, uint8_t fill) :
    id(id), ext(ext), size(data.size()), data(CopyData(data, capacity, fill)),
    capacity_(capacity > data.size() ? capacity : data.size()), free_(true) {}

Frame::Frame(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity, bool free) :
    id(id), ext(ext), size(size), data(data),
    capacity_(capacity > size ? capacity : size), free_(free) {}

Frame::Frame(const Frame& frame) : id(frame.id), ext(frame.ext), size(frame.size),
    data(frame.free_ ? CopyData(frame.data, frame.size, frame.capacity_) : frame.data),
    capacity_(frame.capacity_), free_(frame.free_) {}

Frame::~Frame() {
    if (free_) {
        delete data;
    } 
}

Frame Frame::Copy(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity, uint8_t fill) {
    return Frame(id, ext, CopyData(data, size, capacity, fill), size, capacity, true);
}

Frame Frame::Wrap(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity) {
    if (size > capacity) {
        capacity = size;
    }
    return Frame(id, ext, data, size, capacity, false);
}

void Frame::Clear(uint8_t fill) {
    memset(data, fill, capacity_);
}

void Frame::Set(uint8_t id, uint8_t ext, uint8_t size, uint8_t fill) {
    this->id = id;
    this->ext = ext;
    if (size != 0) {
        this->size = size;
    }
    Clear(fill);
}

uint8_t Frame::Capacity() const {
    return capacity_;
}

size_t Frame::printTo(Print& p) const {
    size_t n = 0;
    n += p.print(id, HEX);
    n += p.print("#");
    for (int i = 0; i < size; i++) {
        if (data[i] <= 0x0F) {
            n += p.print("0");
        }
        n += p.print(data[i], HEX);
        if (i < size-1) {
            n += p.print(":");
        }
    }
    return n;
}

bool operator==(const Frame& left, const Frame& right) {
    if (left.id != right.id || left.size != right.size || left.ext != right.ext) {
        return false;
    }
    if (left.data == right.data) {
        return true;
    }
    return memcmp(left.data, right.data, left.size) == 0;
}

bool operator!=(const Frame& left, const Frame& right) {
    return !(left == right);
}

}  // namespace Canny
