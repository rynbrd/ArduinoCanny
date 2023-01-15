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

template <typename FrameType>
bool MCP2518<FrameType>::begin(Bitrate bitrate) {
    bitrate_ = bitrate;
    mode_ = internal::getMode(bitrate_);
    ready_ = false;
    if (mcp_.begin(GetMCP2518Bitrate(bitrate)) == CAN_OK) {
        mcp_.setMode(CAN_NORMAL_MODE);
        ready_ = true;
    }
    return ready_;
}

template <typename FrameType>
Mode MCP2518<FrameType>::mode() const {
    return mode_;
}

template <typename FrameType>
Bitrate MCP2518<FrameType>::bitrate() const {
    return bitrate_;
}

template <typename FrameType>
Error MCP2518<FrameType>::read(FrameType* frame) {
    if (!ready_) {
        return ERR_READY;
    }

    if (mcp_.checkReceive() != CAN_MSGAVAIL) {
        return ERR_FIFO;
    }
    if (mcp_.readMsgBuf(frame->mutable_size(), frame->data(), frame->capacity()) != CAN_OK) {
        return ERR_INTERNAL;
    }
    frame->id(mcp_.getCanId());
    frame->ext(mcp_.isExtendedFrame());
    return ERR_OK;
}

template <typename FrameType>
Error MCP2518<FrameType>::write(const FrameType& frame) {
    if (!ready_) {
        return ERR_READY;
    }
    if (frame.data() == nullptr || (mode_ == CAN20 && frame.size() > 8) || frame.size() > 64) {
        return ERR_INVALID;
    }

    uint8_t size = frame.size();
    if (mode_ == CANFD_CONST_RATE || mode_ == CANFD_DUAL_RATE) {
        size = CANFD::len2dlc(size);
    }
    if (mcp_.sendMsgBuf(frame.id(), frame.ext(), size, frame.data()) == CAN_OK) {
        return ERR_OK;
    }
    return ERR_INTERNAL;
}

template <typename FrameType>
void MCP2518<FrameType>::setFilter(uint8_t num, uint8_t ext, uint32_t filter, uint32_t mask) {
    mcp_.init_Filt_Mask(num, ext, filter, mask);
}

template <typename FrameType>
void MCP2518<FrameType>::clearFilter(uint8_t num) {
    mcp_.CANFDSPI_FilterDisable((CAN_FILTER)num);
}

template <typename FrameType>
void MCP2518<FrameType>::disableFilters() {
    for (uint8_t i = 0; i < 32; ++i) {
        clearFilter(i);
    }
}

}  // namespace Canny
