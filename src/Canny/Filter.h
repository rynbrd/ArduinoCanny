#ifndef _CANNy_FILTER_H_
#define _CANNy_FILTER_H_

#include <Arduino.h>
#include "Frame.h"

namespace Canny {

enum class FilterMode : uint8_t {
    ALLOW,  // All frames are allowed by default.
    DROP,   // All frames are dropped by default.
};

// A configurable filter that filters frames by ID.
class FrameIDFilter {
    public:
        FrameIDFilter(FilterMode mode = FilterMode::ALLOW) :
            mode_(mode), items_(nullptr), size_(0), len_(0) {}
        ~FrameIDFilter();

        // Clear the filter and Set the filter mode.
        void mode(FilterMode mode);

        // Allow a frame.
        void allow(uint32_t frame_id);
        void allow(const Frame& frame) { allow(frame.id()); }

        // Drop a frame.
        void drop(uint32_t frame_id);
        void drop(const Frame& frame) { drop(frame.id()); }

        // Clear the filter.
        void clear();

        // Return true if a frame is allowed through the filter.
        bool match(uint32_t frame_id);
        bool match(const Frame& frame) { return match(frame.id()); }

    private:
        // Add a frame ID to filter items.
        void add(uint32_t frame_id);

        // Remove a frame ID from filter items.
        void remove(uint32_t frame_id);

        // Reserve capacity for n+1 items. Doubles the capacity when necessary.
        void reserve();

        FilterMode mode_;
        uint32_t* items_;
        size_t size_;
        size_t len_;
};

}  // namespace Canny

#endif  // _CANNy_FILTER_H_
