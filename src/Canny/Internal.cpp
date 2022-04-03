#include "Internal.h"

namespace Canny {
namespace internal {

Mode GetMode(Bitrate bitrate) {
    if (bitrate < CANFD_125K) {
        return CAN20;
    }
    if (bitrate < CANFD_125K_500K) {
        return CANFD_CONST_RATE;
    }
    return CANFD_DUAL_RATE;
}

}  // namespace internal
}  // namespace Canny
