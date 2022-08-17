#ifndef _CANNY_ERROR_H_
#define _CANNY_ERROR_H_

namespace Canny {

// Errors returend by read and write operations.
enum Error : uint8_t {
    ERR_OK = 0,         // No error.
    ERR_FIFO = 1,       // No frame available for read or no space to write.
    ERR_READY = 2,      // Controller or network not ready.
    ERR_INVALID = 3,    // Invalid arguments.
    ERR_INTERNAL = 4,   // Internal error.
};

}  // namespace Canny

#endif // _CANNY_ERROR_H_
