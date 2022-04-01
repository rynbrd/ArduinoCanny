#ifndef _CANBED_UTIL_H_
#define _CANBED_UTIL_H_

#include "controller.h"

namespace CANBed {
namespace internal {

// Get the mode from the provided bitrate.
Mode GetMode(Bitrate bitrate);

}
}

#endif  // _CANBED_UTIL_H_
