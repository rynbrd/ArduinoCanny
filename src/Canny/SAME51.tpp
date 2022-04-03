// This is implemented as a TPP header file to avoid building this code unless
// it's used. This is necessary for SAME51 as it is included in the Arduino
// package instead of as a separate library. It is also helpful for efficiency
// when the controller is present but not needed.

#include "Internal.h"

namespace Canny {
namespace {

Bitrate FixSAME51Bitrate(Bitrate bitrate) {
    switch(bitrate) {
        case CANFD_500K_4M:
        case CANFD_500K_5M:
        case CANFD_500K_6M5:
        case CANFD_500K_8M:
        case CANFD_500K_10M:
            return CANFD_500K_4M;
        case CANFD_1000K_4M:
        case CANFD_1000K_8M:
            return CANFD_1000K_4M;
        default:
            return bitrate;
    }
}

uint32_t GetSAME51Bitrate(Bitrate bitrate) {
    switch(bitrate) {
        case CAN20_125K:
        case CANFD_125K:
            return CAN_125KBPS;
        default:
        case CAN20_250K:
        case CANFD_250K:
            return CAN_250KBPS;
        case CAN20_500K:
        case CANFD_500K:
            return CAN_500KBPS;
        case CAN20_1000K:
        case CANFD_1000K:
            return CAN_1000KBPS;
        case CANFD_125K_500K:
            return CAN_125K_500K;
        case CANFD_250K_500K:
            return CAN_250K_500K;
        case CANFD_250K_750K:
            return CAN_250K_750K;
        case CANFD_250K_1M:
            return CAN_250K_1M;
        case CANFD_250K_1M5:
            return CAN_250K_1M5;
        case CANFD_250K_2M:
            return CAN_250K_2M;
        case CANFD_250K_3M:
            return CAN_250K_3M;
        case CANFD_250K_4M:
            return CAN_250K_4M;
        case CANFD_500K_1M:
            return CAN_500K_1M;
        case CANFD_500K_2M:
            return CAN_500K_2M;
        case CANFD_500K_3M:
            return CAN_500K_3M;
        case CANFD_500K_4M:
            return CAN_500K_4M;
        case CANFD_1000K_4M:
            return CAN_1000K_4M;
    }
}

}  // namespace

bool SAME51::begin(Bitrate bitrate) {
    bitrate_ = FixSAME51Bitrate(bitrate);
    mode_ = internal::GetMode(bitrate_);
    return same51_.begin(MCP_ANY, GetSAME51Bitrate(bitrate_), MCAN_MODE_CAN) == CAN_OK;
}

Mode SAME51::mode() const {
    return mode_;
}

Bitrate SAME51::bitrate() const {
    return bitrate_;
}

Error SAME51::read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) {
    if (!ready_) {
        return ERR_READY;
    }

    switch (same51_.readMsgBuf(id, ext, data, size)) {
        case CAN_OK:
            return ERR_OK;
        case CAN_NOMSG:
            return ERR_FIFO;
        default:
            return ERR_INTERNAL;
    }
}

Error SAME51::write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) {
    if (!ready_) {
        return ERR_READY;
    }
    if (data == nullptr || (mode_ == CAN20 && size > 8) ||
            ((mode_ == CANFD_CONST_RATE || mode_ == CANFD_DUAL_RATE) && size > 64)) {
        return ERR_INVALID;
    }
    if (ext > 1) {
        ext = 1;
    }

    switch (same51_.sendMsgBuf(id, ext, size, data)) {
        case CAN_OK:
            return ERR_OK;
        case CAN_FAILTX:
            return ERR_FIFO;
        default:
            return ERR_INTERNAL;
    }
}

}  // namespace Canny
