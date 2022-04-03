#ifndef _CANNY_INTERNAL_H_
#define _CANNY_INTERNAL_H_

#include "Controller.h"

namespace Canny {
namespace internal {

// Get the mode from the provided bitrate.
Mode GetMode(Bitrate bitrate);

}  // namespace internal
}  // namespace Canny

#endif  // _CANNY_INTERNAL_H_
