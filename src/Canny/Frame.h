#ifndef _CANNY_FRAME_H_
#define _CANNY_FRAME_H_

#include <Arduino.h>

namespace Canny {

class Frame : public Printable {
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

        // Construct an empty frame with no capacity. data is set to nullptr.
        Frame();

        // Construct an empty CAN frame with the specified capacity. Each byte
        // of the data capacity is set to fill.
        Frame(uint8_t capacity, uint8_t fill=0x00);

        // Construct a CAN frame with the provided values and capacity.
        // Capacity is set to size if no capacity is provided. If capacity is
        // greater than size the remainder is set to the value of fill.
        Frame(uint32_t id, uint8_t ext, uint8_t size, uint8_t capacity=0, uint8_t fill=0x00);

#ifdef EPOXY_DUINO
        // Construct a CAN frame with the provided values, data, and capacity.
        // The size is set to the length of data. Capacity is set to size if no
        // capacity is provided. If capacity is greater than size the remainder
        // is set to the value of fill.
        Frame(uint32_t id, uint8_t ext, std::initializer_list<uint8_t> data, uint8_t capacity=0,
                uint8_t fill=0x00);
#endif

        // Copy constructor. If the source frame owns its data then the new
        // frame will hold a copy of that data. It is freed when the frame is
        // freed. Otherwise the pointer to the unowned data is copied.
        Frame(const Frame& frame);

        // Free's the memory pointed to by data if the frame owns that memory.
        ~Frame();

        // Create a new CAN frame with the set of provided values and a copy of
        // the provided data. The capacity of the frame is set to max(size,
        // capacity). Extra capacity is padded with the fill value which
        // defaults to 0x00.
        static Frame copy(uint32_t id, uint8_t ext, uint8_t* data,
                uint8_t size, uint8_t capacity = 0, uint8_t fill=0x00);

        // Wrap a CAN frame around a set of frame values and a pointer to the
        // frame's data. The new frame does not take ownership of the memory
        // pointed to by data. The capacity of the frame is set to
        // max(size, capacity). Capacity should not exceed the size of memory
        // allocated to data.
        static Frame wrap(uint32_t id, uint8_t ext, uint8_t* data,
                uint8_t size, uint8_t capacity = 0);

        // Clear the frame data. Its bytes set to 0x00 unless fill is set to
        // another value. This would be 0xFF for J1939 and some other
        // protocols.
        void clear(uint8_t fill=0x00);

        // Convenience method for setting the frame values and clearing the
        // frame data. The data bytes are set to 0x00 unless fill is set to
        // another value.
        void set(uint8_t id, uint8_t ext, uint8_t size=0, uint8_t fill=0x00);

        // Return the capacity of the frame.
        uint8_t capacity() const;

        // Write a human readable string representation of the frame to a
        // print object. Return the number of bytes written. Implements
        // Printable.
        size_t printTo(Print& p) const override;

    private:
        // Construct a CAN frame from the provided arguments.
        Frame(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size, uint8_t capacity, bool free);

        // The capacity of the frame. This should generally be 8 for CAN 2.0
        // frames and 64 for CAN FD frames.
        const uint8_t capacity_;
        // Whether or not to free the memory pointed to by data.
        const bool free_;

        friend bool operator==(const Frame&, const Frame&);
};

// Return true if the values of two frames are equal. The id, ext, size, and
// data are compared directly. Only data[:size] is compared. The capacities of
// the two frames are ignored.
bool operator==(const Frame& left, const Frame& right);

// Return true if the values of two frames are not equal.
// Equivalent to !(left == right).
bool operator!=(const Frame& left, const Frame& right);

}  // namespace Canny

#endif  // _CANNY_FRAME_H_
