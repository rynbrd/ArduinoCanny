#ifndef _CANNY_CONTROLLER_H_
#define _CANNY_CONTROLLER_H_

#include <Arduino.h>
#include "Connection.h"

namespace Canny {

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

// Base class for all CAN controllers. A CAN controller is a physical
// tranceiver connected to a CAN bus.
class Controller : public Connection {
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
};

}

#endif  // _CANNY_CONTROLLER_H_
