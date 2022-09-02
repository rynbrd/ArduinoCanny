#include "Filter.h"

#include <Arduino.h>

namespace Canny {

FrameIDFilter::~FrameIDFilter() {
    if (items_ != nullptr) {
        delete[] items_;
    }
}

void FrameIDFilter::mode(FilterMode mode) {
    clear();
    mode_ = mode;
}

void FrameIDFilter::allow(uint32_t frame_id) {
    if (mode_ == FilterMode::ALLOW) {
        Serial.println("allow -> remove");
        remove(frame_id);
    } else {
        Serial.println("allow -> add");
        add(frame_id);
    }
}

void FrameIDFilter::drop(uint32_t frame_id) {
    if (mode_ == FilterMode::ALLOW) {
        Serial.println("drop -> add");
        add(frame_id);
    } else {
        Serial.println("drop -> remove");
        remove(frame_id);
    }
}

void FrameIDFilter::add(uint32_t frame_id) {
    reserve();
    items_[len_++] = frame_id;
    Serial.print("added "); Serial.println(frame_id, HEX);
    for (size_t i = 0; i < len_; i++) {
        Serial.print("  item: "); Serial.println(items_[i], HEX);
    }
}

void FrameIDFilter::remove(uint32_t frame_id) {
    for (size_t i = 0; i < len_; i++) {
        while (items_[i] == frame_id) {
            Serial.println("removing");
            memcpy(items_ + i, items_ + i + 1, (len_ - i - 1) * sizeof(uint32_t));
            --len_;
        }
    }
    Serial.print("removed "); Serial.println(frame_id, HEX);
    for (size_t i = 0; i < len_; i++) {
        Serial.print("  item: "); Serial.println(items_[i], HEX);
    }
}

void FrameIDFilter::clear() {
    len_ = 0;
}

bool FrameIDFilter::match(uint32_t frame_id) {
    for (size_t i = 0; i < len_; i++) {
        if (items_[i] == frame_id) {
            Serial.print("matched "); Serial.println(frame_id, HEX);
            return mode_ != FilterMode::ALLOW;
        }
    }
    return mode_ == FilterMode::ALLOW;
}

void FrameIDFilter::reserve() {
    if (len_ + 1 <= size_) {
        return;
    }

    size_t new_size = size_ == 0 ? 1 : size_ * 2;
    uint32_t* new_items = new uint32_t[new_size];
    if (items_ != nullptr) {
        memcpy(new_items, items_, len_ * sizeof(uint32_t));
        delete[] items_;
    }
    items_ = new_items;
    size_ = new_size;
}

}  // namespace Canny
