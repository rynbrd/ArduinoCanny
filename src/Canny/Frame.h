#ifndef _CANNY_FRAME_H_
#define _CANNY_FRAME_H_

#include <Arduino.h>

namespace Canny {

class Frame : public Printable {
    public:
        // Construct an empty frame with no capacity. Its data is set to
        // nullptr and its capacity 0.
        Frame();

        // Copy constructor. The frame's values and data are copied to the new
        // frame. This can be an expensive operation on slower boards.
        Frame(const Frame& frame);

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

        // Free's the memory pointed to by data if the frame owns that memory.
        ~Frame();

        // Return the ID of the frame. This is an 11-bit value for standard
        // frames and a 29-bit value for extended frames.
        uint32_t id() const { return id_; }

        // Set the ID of the frame.
        void id(uint32_t id) { id_ = id; }

        // Set the ID of the frame. The ext argument must be set to 1 to
        // indicate that the ID is a 29-bit identifier. Otherwise it is treated
        // as 11-bits.
        void id(uint32_t id, uint8_t ext);

        // Return 1 if the frame ID is 29-bit extended identifier. Other values
        // indicate a 11-bit standard identifier.
        uint8_t ext() const { return ext_; }

        // Set the ext property of the frame. If ext is 1 then the frame holds
        // an extended (29-bit) identifier. Otherwise it holds a standard
        // (11-bit) identifier.
        void ext(uint8_t ext) { ext_ = (ext == 1) ? 1 : 0; } 

        // Return the length of the frame's data in bytes.
        uint8_t size() const { return size_; }

        // Return the capacity of the frame's data buffer.
        uint8_t capacity() const { return capacity_; }

        // Return a pointer to the frame's data. The data is mutable and is at
        // least capacity() bytes long. Return nullptr if capacity is 0.
        uint8_t* data() const { return data_; }

        // Ensures the frame has at least the requested amount of capacity.
        // Expands the frame's storage to match capacity if necessary. New
        // bytes are set to fill.
        void reserve(uint8_t capacity, uint8_t fill=0x00);

        // Resize the frame's data. Reserves space using the provided fill
        // value to ensure that enough capacity exists to support the new size.
        // Reducing a frame's size does not reduce its capacity.
        void resize(uint8_t size, uint8_t fill=0x00);

        // Clear the frame data. Its bytes set to 0x00 unless fill is set to
        // another value. This would be 0xFF for J1939 and some other
        // protocols.
        void clear(uint8_t fill=0x00);

        // Write a human readable string representation of the frame to a
        // print object. Return the number of bytes written. Implements
        // Printable.
        size_t printTo(Print& p) const override;

        // Copy the contents of one frame to another. The capacity of the
        // destination frame is expanded if it is less than the size of the
        // source frame.
        Frame& operator=(const Frame& frame);

    private:
        // The ID of the frame. This is an 11-bit value for standard frames and
        // a 29-bit value for extended frames.
        uint32_t id_;
        // Set to 0 for standard frame or 1 for extended frame.
        uint8_t ext_;
        // The size of the data in the frame.
        uint8_t size_;
        // The capacity of the frame. This should generally be 8 for CAN 2.0
        // frames and 64 for CAN FD frames.
        uint8_t capacity_;
        // The data transmitted with this frame.
        uint8_t* data_;

        // Return a mutable pointer to the frame's ID. This is used by
        // controller implementations to efficiently set the frame's ID.
        inline uint32_t* mutable_id() { return &id_; }

        // Return a mutable pointer to the frame's ext property. This is used
        // by controller implementations to efficiently set the frame's ext.
        uint8_t* mutable_ext() { return &ext_; }

        // Return a mutable pointer to the frame's size property. This is used
        // by controller implementations to efficiently set the frame's size.
        uint8_t* mutable_size() { return &size_; }

        friend bool operator==(const Frame&, const Frame&);
        friend class Controller;
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
