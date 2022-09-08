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

        // Read a Frame object from the connection. This reserves the required
        // capacity in the frame before reading data into it. This extra
        // overhead can be avoided by allocating a frame with sufficient
        // capacity ahead of time. In that case the reserve is a noop.
        //
        // When the controller is in CAN 2.0 operating mode the frame requires
        // a capacity of at least 8 bytes. CAN FD requires 64 bytes.
        //
        // Return ERR_OK if data is read into the provided frame. Return
        // ERR_FIFO if no frame is available to be read. See the Error
        // definition for the meaning of other error codes.
        virtual Error read(Frame* frame) = 0;

        // Write a frame without blocking.
        //
        // Return ERR_OK on success. Return ERR_FIFO if the internal write
        // buffer is out of space or if the bus would block (the controller
        // fails the priority check during arbitrarion). Return ERR_INVALID if
        // the buffer size is not supported by the controller. CAN 2.0
        // controllers may not transmit buffers larger than 8 bytes. CAN FD
        // controllers are limited to 64 bytes. See the Error definition for
        // the meaning of other error codes.
        virtual Error write(const Frame& frame) = 0;
};

}  // namespace Canny

#endif  // _CANNY_CONNECTION_H_
