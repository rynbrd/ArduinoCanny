#include "Frame.h"

#include <Arduino.h>

namespace Canny {

Frame::Frame() : id_(0), ext_(0), size_(0), capacity_(0), pad_(0x00), data_(nullptr) {}

Frame::Frame(const Frame& frame) :
        id_(frame.id_), ext_(frame.ext_),
        size_(frame.size_), capacity_(frame.size_),
        pad_(frame.pad_), data_(new uint8_t[capacity_]) {
    memcpy(data_, frame.data_, capacity_);
}

Frame::Frame(uint8_t capacity, uint8_t pad) :
        id_(0), ext_(0), size_(0), capacity_(0), pad_(pad), data_(nullptr) {
    reserve(capacity);
}

Frame::Frame(uint32_t id, uint8_t ext, uint8_t size, uint8_t capacity, uint8_t pad) :
        id_(id), ext_(ext), size_(size), capacity_(0), pad_(pad), data_(nullptr) {
    reserve(max(size, capacity));
}

Frame::~Frame() {
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    } 
}

void Frame::id(uint32_t id, uint8_t ext) {
    id_ = id;
    ext_ = (ext == 1) ? 1 : 0;
}

void Frame::data(const uint8_t* data, uint8_t len) {
    resize(len);
    memcpy(data_, data, len);
}


void Frame::reserve(uint8_t capacity) {
    if (capacity <= capacity_) {
        return;
    }

    uint8_t* new_data = new uint8_t[capacity];
    if (data_ != nullptr) {
        memcpy(new_data, data_, capacity_);
        memset(new_data+capacity_, pad_, capacity-capacity_);
        delete[] data_;
    } else {
        memset(new_data, pad_, capacity);
    }
    capacity_ = capacity;
    data_ = new_data;
}

void Frame::resize(uint8_t size) {
    if (size < capacity_) {
        memset(data_+size, pad_, capacity_-size);
    }
    reserve(size);
    size_ = size;
}

void Frame::clear() {
    memset(data_, pad_, capacity_);
}

void Frame::clear(uint8_t pad) {
    pad_ = pad;
    clear();
}

size_t Frame::printTo(Print& p) const {
    size_t n = 0;
    if (ext_) {
        n += p.print("+");
    } else {
        n += p.print("-");
    }
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
            delete[] data_;
        }
        data_ = new uint8_t[size_];
        capacity_ = size_;
    }
    memcpy(data_, other.data_, size_);
    return *this;
}

bool operator==(const Frame& left, const Frame& right) {
    if (left.id_ != right.id_ || left.size_ != right.size_ || left.ext_ != right.ext_) {
        return false;
    }
    if (left.data_ == right.data_) {
        return true;
    }
    if (left.data_ == nullptr || right.data_ == nullptr) {
        return false;
    }
    return memcmp(left.data_, right.data_, left.size_) == 0;
}

bool operator!=(const Frame& left, const Frame& right) {
    return !(left == right);
}

}  // namespace Canny
