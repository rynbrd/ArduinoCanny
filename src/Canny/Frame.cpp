#include "Frame.h"

#include <Arduino.h>

namespace Canny {

Frame::Frame() : id_(0), ext_(0), size_(0), capacity_(0), data_(nullptr) {}

Frame::Frame(const Frame& frame) :
        id_(frame.id_), ext_(frame.ext_),
        size_(frame.size_), capacity_(frame.capacity_),
        data_(new uint8_t[capacity_]) {
    memcpy(data_, frame.data_, capacity_);
}

Frame::Frame(uint8_t capacity, uint8_t fill) : Frame() {
    reserve(capacity, fill);
}

Frame::Frame(uint32_t id, uint8_t ext, uint8_t size, uint8_t capacity, uint8_t fill) :
        id_(id), ext_(ext), size_(size), data_(nullptr) {
    reserve(max(size, capacity), fill);
}

#ifdef EPOXY_DUINO
Frame::Frame(uint32_t id, uint8_t ext, std::initializer_list<uint8_t> data, uint8_t capacity,
        uint8_t fill) : id_(id), ext_(ext), size_(data.size()),
        capacity_(max((uint8_t)data.size(), capacity)),
        data_(new uint8_t[capacity_]) {
    uint8_t i = 0;
    for (auto it = data.begin(); it != data.end(); it++) {
        data_[i++] = *it;
    }
    if (capacity > data.size()) {
        memset(data_+data.size(), fill, capacity-data.size());
    }
}
#endif

Frame::~Frame() {
    if (data_ != nullptr) {
        delete data_;
        data_ = nullptr;
    } 
}

void Frame::reserve(uint8_t capacity, uint8_t fill) {
    if (capacity <= capacity_) {
        return;
    }

    uint8_t* new_data = new uint8_t[capacity];
    if (data_ != nullptr) {
        memcpy(new_data, data_, capacity_);
        delete data_;
    }
    memset(new_data+capacity_, fill, capacity-capacity_);
    capacity_ = capacity;
    data_ = new_data;
}

void Frame::resize(uint8_t size, uint8_t fill) {
    reserve(size, fill);
    size_ = size;
}

void Frame::clear(uint8_t fill) {
    memset(data_, fill, capacity_);
}

size_t Frame::printTo(Print& p) const {
    size_t n = 0;
    n += p.print(id_, HEX);
    n += p.print("#");
    for (int i = 0; i < size_; i++) {
        if (data_[i] <= 0x0F) {
            n += p.print("0");
        }
        n += p.print(data_[i], HEX);
        if (i < size_-1) {
            n += p.print(":");
        }
    }
    return n;
}

Frame& Frame::operator=(const Frame& other) {
    id_ = other.id_;
    ext_ = other.ext_;
    size_ = other.size_;
    if (capacity_ < other.size_) {
        if (data_ != nullptr) {
            delete data_;
        }
        data_ = new uint8_t[size_];
        capacity_ = size_;
    }
    memcpy(data_, other.data_, size_);
}

bool operator==(const Frame& left, const Frame& right) {
    if (left.id_ != right.id_ || left.size_ != right.size_ || left.ext_ != right.ext_) {
        return false;
    }
    if (left.data_ == right.data_) {
        return true;
    }
    return memcmp(left.data_, right.data_, left.size_) == 0;
}

bool operator!=(const Frame& left, const Frame& right) {
    return !(left == right);
}

}  // namespace Canny
