#ifndef _CANNY_FRAME_H_
#define _CANNY_FRAME_H_

#include <Arduino.h>

namespace Canny {

// A CAN frame. CAN frames have an ID, extended frame format flag, and a
// payload. The payload is represented by a byte array of varying capacity and
// sizes. Capacity is static for a particular frame implementation, but size
// may vary from zero to capacity. Capacity of a frame matches the maximum
// poayload size of the frame on the wire. For CAN 2.0 this is 8 bytes. For
// CanFD this is 64 bytes. Newly allocated and unused capacity is padded with a
// given pad byte.
template <size_t Capacity>
class Frame : public Printable {
    public:
        static constexpr size_t capacity = Capacity;

        // Construct an empty frame.
        Frame();

        // Construct an empty frame with the specified size. The size is
        // truncated to the frame's capacity and the frame's data is filled
        // with the pad byte.
        explicit Frame(uint8_t size, uint8_t pad=0x00);

        // Construct a CAN frame with the provided values and size. The size is
        // truncated to the frame's capacity and the frame's data is filled
        // with the pad byte.
        Frame(uint32_t id, uint8_t ext, uint8_t size, uint8_t pad=0x00);

        // Construct a CAN frame with the provided values and data. Data is
        // truncated to the frame's capacity and the size is set to the
        // resulting data length. Any remaining capacity is filled with pad
        // bytes.
        template <size_t N> 
        Frame(uint32_t id, uint8_t ext, const uint8_t (&data)[N], uint8_t pad=0x00);

        // Defaults destructor.
        virtual ~Frame() = default;

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

        // Return a pointer to the frame's data. The data is mutable and is
        // exactly capacity() bytes long. Never returns a nullptr.
        uint8_t* data() const { return (uint8_t*)data_; }

        // Set the data from the provided initializer list. Data is truncated
        // to Capacity and the frame's size is set to the resulting length.
        template <size_t N> 
        void data(const uint8_t (&data)[N]);

        // Set the data from the provided buffer. Data is truncated to Capacity
        // and the frame's size is set to the resulting length.
        void data(const uint8_t* data, uint8_t len);

        // Return a mutable pointer to the frame's ID. This is used by
        // controller implementations to efficiently set the frame's ID.
        uint32_t* mutable_id() { return &id_; }

        // Return a mutable pointer to the frame's ext property. This is used
        // by controller implementations to efficiently set the frame's ext.
        uint8_t* mutable_ext() { return &ext_; }

        // Return a mutable pointer to the frame's size property. This is used
        // by controller implementations to efficiently set the frame's size.
        // The resize() method should be preferred as it is safer.
        uint8_t* mutable_size() { return &size_; }

        // Resize the frame's data. Size is truncated to Capacity. Any
        // remaining capacity is overwritten with pad bytes.
        void resize(uint8_t size);

        // Clear the frame data. Its bytes are set to the stored pad value. The
        // size does not change.
        void clear();

        // Clear the frame data and upate the pad value. The frame bytes set to
        // the new pad value.
        void clear(uint8_t pad);

        // Write a human readable string representation of the frame to a
        // print object. Return the number of bytes written. Implements
        // Printable.
        size_t printTo(Print& p) const override;

        // Copy the contents of one frame to another. If the destination frame
        // does not have the capacity to store the source's data then the data
        // is truncated to fit.
        template <size_t OtherCapacity>
        Frame<Capacity>& operator=(const Frame<OtherCapacity>& frame);
    private:
        // The ID of the frame. This is an 11-bit value for standard frames and
        // a 29-bit value for extended frames.
        uint32_t id_;
        // Set to 0 for standard frame or 1 for extended frame.
        uint8_t ext_;
        // The size of the data in the frame.
        uint8_t size_;
        // The byte to pad extra capacity with.
        uint8_t pad_;
        // The data transmitted with this frame.
        uint8_t data_[Capacity];
};

// Return true if the values of two frames are equal. The id, ext, size, and
// data are compared directly. Only data[:size] is compared. The capacities of
// the two frames are ignored.
template <size_t LeftCapacity, size_t RightCapacity>
bool operator==(const Frame<LeftCapacity>& left, const Frame<RightCapacity>& right);

// Return true if the values of two frames are not equal.
// Equivalent to !(left == right).
template <size_t LeftCapacity, size_t RightCapacity>
bool operator!=(const Frame<LeftCapacity>& left, const Frame<RightCapacity>& right);

typedef Frame<8> CAN20Frame;
typedef Frame<64> CANFDFrame;

}  // namespace Canny

#include "Frame.tpp"

#endif  // _CANNY_FRAME_H_
