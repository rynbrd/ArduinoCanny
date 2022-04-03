#ifndef _CANNY_FRAME_H_
#define _CANNY_FRAME_H_

#include <Arduino.h>

namespace Canny {

class Frame {
    public:
        // The ID of the frame. This is an 11-bit value for standard frames and
        // a 29-bit value for extended frames.
        uint32_t id;
        // Set to 0 for standard frame or 1 for extended frame.
        uint8_t ext;
        // The size of the data in the frame.
        uint8_t size;
        // The data transmitted with this frame.
        uint8_t* const data;

        // Construct an empty CAN frame with the specified capacity.
        Frame(size_t capacity);

        // Copy constructor. If the source frame owns its data then the new
        // frame will hold a copy of that data. It is freed when the frame is
        // freed. Otherwise the pointer to the unowned data is copied.
        Frame(const Frame& frame);

        // Free's the memory pointed to by data if the frame owns that memory.
        ~Frame();

        // Create a new CAN frame with the set of provided values and a copy of
        // the provided data. The capacity of the frame is set to max(size,
        // capacity).
        static Frame Copy(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity = 0);

        // Wrap a CAN frame around a set of frame values and a pointer to the
        // frame's data. The new frame does not take ownership of the memory
        // pointed to by data. The capacity of the frame is set to
        // max(size, capacity).
        static Frame Wrap(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity = 0);

        // Return the capacity of the frame.
        uint8_t Capacity() const;

    private:
        // Construct a CAN frame from the provided arguments.
        Frame(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity, bool free);

        // The capacity of the frame. This should generally be 8 for CAN 2.0
        // frames and 64 for CAN FD frames.
        const uint8_t capacity_;
        // Whether or not to free the memory pointed to by data.
        const bool free_;
};

}  // namespace Canny

#endif  // _CANNY_FRAME_H_
