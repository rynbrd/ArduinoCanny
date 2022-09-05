#ifndef _CANNY_CONNECTION_H_
#define _CANNY_CONNECTION_H_

#include <Arduino.h>
#include "Error.h"
#include "Frame.h"

namespace Canny {

// Base class for all CAN connections. This is a generalized class for sending
// and receiving CAN frames. A connection does not necessarily represent a
// connection to a physical CAN bus but may instead be used to communicate
// frames with other devices.
class Connection {
    public:
        Connection() = default;
        virtual ~Connection() = default;

        // Read a frame without blocking. The id is set with the ID of the
        // frame. The ext argument is not null then it is set to 1 when the
        // ID is an extended (29-bit) identifier or 0 when it is a standard
        // (11-bit) identifier. The frame data is written into the provided
        // data buffer. It must already be allocated to a size appropriate
        // to the operating mode of the connection. For CAN 2.0 this is 8
        // bytes. For CAN FD this is 64 bytes. The size is set to the number of
        // bytes written to the buffer and matches the size of the frame's
        // data.
        //
        // Return ERR_OK if data is read into the provided buffers. Return
        // ERR_FIFO if no frame is available to be read. See the Error
        // definition for the meaning of other error codes.
        virtual Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) = 0;

        // A variant of read that operates on a Frame object. This is
        // marginally safer than read(id, ...) because it reserves the required
        // capacity in the frame before reading data into it. The extra
        // overhead can be avoided by allocating a frame with sufficient
        // capacity ahead of time. In this case the reserve is a noop.
        //
        // When the controller is in CAN 2.0 operating mode the frame requires
        // a capacity of at least 8 bytes. CAN FD requires 64 bytes.
        virtual Error read(Frame* frame) = 0;

        // Write a frame without blocking. If ext is 0 then id is treated as
        // a standard (11-bit) identifier; otherwise it is an extended (29-bit)
        // identifier. The data buffer must be at least size bytes long.
        //
        // Return ERR_OK on success. Return ERR_FIFO if the internal write
        // buffer is out of space or if the bus would block (the controller
        // fails the priority check during arbitrarion). Return ERR_INVALID if
        // the buffer size is not supported by the controller. CAN 2.0
        // controllers may not transmit buffers larger than 8 bytes. CAN FD
        // controllers are limited to 64 bytes. See the Error definition for
        // the meaning of other error codes.
        virtual Error write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) = 0;

        // A variant of write that operates on a Frame object. Works identical
        // to write(id, ...). 
        virtual Error write(const Frame& frame) = 0;
};

// A connection which wraps another connection and filters reads and writes.
class FilteredConnection : public Connection {
    public:
        FilteredConnection(Connection* child) : child_(child) {}

        // Read af rame. Return ERR_FIFO for filtered frames or the result of
        // the child's read call.
        Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) override;
        Error read(Frame* frame) override;

        // Write a frame. Return ERR_OK if it was filtered or the result of the
        // child's write call.
        Error write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) override;
        Error write(const Frame& frame) override;

        // Return true for frames which should be returned by calls to read().
        virtual bool readFilter(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) const = 0;

        // Return true for frames which should be written to the child.
        virtual bool writeFilter(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) const = 0;

    private:
        Connection* child_;
};


}  // namespace Canny

#endif  // _CANNY_CONNECTION_H_
