// This is implemented as a TPP header file to avoid building this code unless
// it's used. This is done for efficiency as a board will only have one or two
// different CAN controllers.

#include "Internal.h"

namespace Canny {
namespace {

uint32_t GetMCP2518Bitrate(Bitrate bitrate) {
    switch(bitrate) {
        case CAN20_125K:
            return CAN20_125KBPS;
        case CAN20_250K:
            return CAN20_250KBPS;
        case CAN20_500K:
            return CAN20_500KBPS;
        case CAN20_1000K:
            return CAN20_1000KBPS;
        case CANFD_125K:
            return 150000UL;
        case CANFD_250K:
            return 250000UL;
        case CANFD_500K:
            return 500000UL;
        case CANFD_1000K:
            return 1000000UL;
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
        case CANFD_500K_5M:
            return CAN_500K_5M;
        case CANFD_500K_6M5:
            return CAN_500K_6M5;
        case CANFD_500K_8M:
            return CAN_500K_8M;
        case CANFD_500K_10M:
            return CAN_500K_10M;
        case CANFD_1000K_4M:
            return CAN_1000K_4M;
        case CANFD_1000K_8M:
            return CAN_1000K_8M;
    }
    return CAN20_250K;
}

}  // namespace

bool MCP2518::begin(Bitrate bitrate) {
    bitrate_ = bitrate;
    mode_ = internal::getMode(bitrate_);
    ready_ = false;
    if (mcp_.begin(GetMCP2518Bitrate(bitrate)) == CAN_OK) {
        mcp_.setMode(CAN_NORMAL_MODE);
        ready_ = true;
    }
    return ready_;
}

Mode MCP2518::mode() const {
    return mode_;
}

Bitrate MCP2518::bitrate() const {
    return bitrate_;
}

Error MCP2518::read(Frame* frame) {
    frame->reserve(64);
    return read(frame->mutable_id(), frame->mutable_ext(), frame->data(), frame->mutable_size());
}

Error MCP2518::read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) {
    if (!ready_) {
        return ERR_READY;
    }
    if (mcp_.checkReceive() != CAN_MSGAVAIL) {
        return ERR_FIFO;
    }
    if (mcp_.readMsgBuf(size, data) != CAN_OK) {
        return ERR_INTERNAL;
    }
    *id = mcp_.getCanId();
    if (ext != nullptr) {
        *ext = mcp_.isExtendedFrame();
    }
    return ERR_OK;
}

Error MCP2518::write(const Frame& frame) {
    return write(frame.id(), frame.ext(), frame.data(), frame.size());
}

Error MCP2518::write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) {
    if (!ready_) {
        return ERR_READY;
    }
    if (data == nullptr || (mode_ == CAN20 && size > 8) ||
            ((mode_ == CANFD_CONST_RATE || mode_ == CANFD_DUAL_RATE) && size > 64)) {
        return ERR_INVALID;
    }
    if (ext > 1)  {
        ext = 1;
    }

    if (mode_ == CANFD_CONST_RATE || mode_ == CANFD_DUAL_RATE) {
        size = CANFD::len2dlc(size);
    }
    if (mcp_.sendMsgBuf(id, ext, size, data) == CAN_OK) {
        return ERR_OK;
    }
    return ERR_INTERNAL;
}

}  // namespace Canny
