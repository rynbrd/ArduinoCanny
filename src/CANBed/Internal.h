#ifndef _CANBED_INTERNAL_H_
#define _CANBED_INTERNAL_H_

#include "Controller.h"

namespace CANBed {
namespace internal {

// Get the mode from the provided bitrate.
Mode GetMode(Bitrate bitrate);

}
}

#endif  // _CANBED_INTERNAL_H_
