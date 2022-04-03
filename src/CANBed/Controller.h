#ifndef _CANBED_CONTROLLER_H_
#define _CANBED_CONTROLLER_H_

#include <Arduino.h>

namespace CANBed {

// Returned by a controller to indicate the operating mode.
enum Mode : uint8_t {
    // CAN 2.0 mode.
    CAN20,
    // CAN FD with constant bitrate.
    CANFD_CONST_RATE,
    // CAN FD with differing bitrates for arbitration and data transmission.
    CANFD_DUAL_RATE,
};

// The available cAN bus bitrates for the different modes. Using a bitrate for
// a particular mode will put the controller into that mode if supported. If
// not then the controller will select the next closest bitrate.
//
// CAN 2.0 controllers will always be placed in CAN20 mode. If a CAN FD bitrate
// is used it will downgrate to the CAN 2.0 bitrate that matches the
// arbitration rate for the selected bitrate.
//
// CAN FD controllers that do not support a selected bitrate will downgrade to the next supported data bitrate using the same arbitration rate.
//
// These adjustments should in many cases allow controllers of differing
// capabilities to communicate on the same bus.
enum Bitrate : uint8_t {
    // Bitrates for CAN 2.0. May be used by CAN 2.0 and CAN FD controllers.
    CAN20_125K,
    CAN20_250K,
    CAN20_500K,
    CAN20_1000K,

    // Constant bitrates for CAN FD.
    CANFD_125K,
    CANFD_250K,
    CANFD_500K,
    CANFD_1000K,

    // Dual bitrates for CAN FD. First number is arbitration rate, second is data
    // rate.
    CANFD_125K_500K,
    CANFD_250K_500K,
    CANFD_250K_750K,
    CANFD_250K_1M,
    CANFD_250K_1M5,
    CANFD_250K_2M,
    CANFD_250K_3M,
    CANFD_250K_4M,
    CANFD_500K_1M,
    CANFD_500K_2M,
    CANFD_500K_3M,
    CANFD_500K_4M,
    CANFD_500K_5M,
    CANFD_500K_6M5,
    CANFD_500K_8M,
    CANFD_500K_10M,
    CANFD_1000K_4M,
    CANFD_1000K_8M,
};

// Errors returend by read and write operations.
enum Error : uint8_t {
    ERR_OK = 0,       // No error.
    ERR_FIFO = 1,       // No frame available for read or no space to write.
    ERR_READY = 2,      // Controller or network not ready.
    ERR_INVALID = 3,    // Invalid arguments.
    ERR_INTERNAL = 4,   // Internal error.
};

// Base class for all CAN controllers.
class Controller {
    public:
        Controller() = default;
        virtual ~Controller() = default;

        // Initialize the controller. The controller must be connected to a bus
        // that supports the provided bitrate. CAN 2.0 controllers only support
        // CAN 2.0 bitrates and will be placed in CAN 2.0 mode. If a CAN FD
        // bitrate is provided to a CAN 2.0 controller then it is configured
        // for the arbitration rate provided by the CAN FD bitrate. CAN FD
        // controllers support both CAN 2.0 and CAN FD mode. Providing a CAN
        // 2.0 bitrate will place the controller into CAN 2.0 mode.
        //
        // Return true on success or false on failure. A failure is not
        // recoverable. 
        virtual bool begin(Bitrate bitrate) = 0;

        // Return the operating mode of the controller. Requires begin() to
        // have already been called.
        virtual Mode mode() const = 0;

        // Return the bitrate of the controller. Requires begin() to have
        // already been called.
        virtual Bitrate bitrate() const = 0;

        // Read a frame without blocking. The id is set with the ID of the
        // frame. The ext argument is not null then it is set to 1 when the
        // ID is an extended (29-bit) identifier or 0 when it is a standard
        // (11-bit) identifier. The frame data is written into the provided
        // data buffer. It must be already be allocated to a size appropriate
        // to the operating mode of the controller. For CAN 2.0 this is 8
        // bytes. For CAN FD this is 64 bytes. The size is set to the number of
        // bytes written to the buffer and matches the size of the frame's
        // data.
        //
        // Return ERR_OK if data is read into the provided buffers. Return
        // ERR_FIFO if no frame is available to be read. See the Error
        // definition for the meaning of other error codes.
        virtual Error read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) = 0;

        // Write a frame without blocking. If ext is 0 then id is treated as
        // a standard (11-bit) identifier; otherwist is is an extended (29-bit)
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
};

}

#endif  // _CANBED_CONTROLLER_H_
