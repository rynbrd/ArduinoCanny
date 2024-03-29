#ifndef _CANNY_REALDASH_H_
#define _CANNY_REALDASH_H_

#include <Arduino.h>
#include <CRC32.h>
#include "Connection.h"
#include "Frame.h"

namespace Canny {

// Reads and writes frames to RealDash over serial. Supports reading RealDash
// 0x44 and 0x66 type frames. All written frames are 0x66 for simplicity.
template <typename FrameType>
class RealDash : public Connection<FrameType> {
    public:
        // Construct a RealDash instance that communicates over the given
        // stream. This is typically Serial or SerialUSB.
        RealDash(Stream* stream);

        // Read a frame from the RealDash stream without blocking. Return
        // ERR_OK when a frame was read or ERR_FIFO when no frame is available.
        // Return ERR_INTERNAL if the serial data is invalid.
        //
        // The ext flag is always set to 1. RealDash encodes all frame IDs as 4
        // bytes and does not provide an ext flag.
        Error read(FrameType* frame) override;

        // Write a frame to the RealDash stream. Return ERR_OK on success.
        virtual Error write(const FrameType& frame) override;

    private:
        Stream* stream_;
        FrameType frame_;

        // Read attributes.
        bool frame_type_66_;        // Type of frame. False if 0x44, true if 0x66.
        uint8_t frame44_checksum_;  // Frame 0x44 checksum. Calculated as bytes are read.
        CRC32::Checksum frame66_checksum_;  // Frame 0x66 checksum. Calculated as bytes are read.
        byte checksum_buffer_[4];   // Buffer to read in the checksum.
        uint8_t read_size_;         // Tracks how many bytes have been read.

        // Write attributes.
        CRC32::Checksum write_checksum_;

        void updateChecksum(byte b);
        bool readHeader();
        bool readId();
        bool readData();
        bool validateChecksum();
        void reset();
        void writeByte(const byte b);
        void writeBytes(const byte* b, uint8_t len);
        void writeBytes(uint32_t data);
};

}  // namespace Canny

#include "RealDash.tpp"

#endif  // _CANNY_REALDASH_H_
