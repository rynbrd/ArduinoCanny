// This is implemented as a TPP header file to avoid building this code unless
// it's used. This is done for efficiency as a board will only have one or two
// different CAN controllers.

namespace Canny {
namespace {

Bitrate FixMCP2515Bitrate(Bitrate bitrate) {
    switch (bitrate) {
        case CAN20_125K:
        case CANFD_125K:
        case CANFD_125K_500K:
            return CAN20_125K;
        case CAN20_250K:
        case CANFD_250K:
        case CANFD_250K_500K:
        case CANFD_250K_750K:
        case CANFD_250K_1M:
        case CANFD_250K_1M5:
        case CANFD_250K_2M:
        case CANFD_250K_3M:
        case CANFD_250K_4M:
            return CAN20_250K;
        case CAN20_500K:
        case CANFD_500K:
        case CANFD_500K_1M:
        case CANFD_500K_2M:
        case CANFD_500K_3M:
        case CANFD_500K_4M:
        case CANFD_500K_5M:
        case CANFD_500K_6M5:
        case CANFD_500K_8M:
        case CANFD_500K_10M:
            return CAN20_500K;
        case CAN20_1000K:
        case CANFD_1000K:
        case CANFD_1000K_4M:
        case CANFD_1000K_8M:
            return CAN20_1000K;
    }
    return CAN20_250K;
} 

uint8_t GetMCP2515Bitrate(Bitrate bitrate) {
    switch (bitrate) {
        case CAN20_125K:
            return CAN_125KBPS;
        default:
        case CAN20_250K:
            return CAN_250KBPS;
        case CAN20_500K:
            return CAN_500KBPS;
        case CAN20_1000K:
            return CAN_1000KBPS;
    }
}

}  // namespace

template <typename FrameType>
bool MCP2515<FrameType>::begin(Bitrate bitrate) {
    bitrate_ = FixMCP2515Bitrate(bitrate);
    ready_ = mcp_.begin(GetMCP2515Bitrate(bitrate_)) == CAN_OK;
    return ready_;
}

template <typename FrameType>
Mode MCP2515<FrameType>::mode() const {
    return CAN20;
}

template <typename FrameType>
Bitrate MCP2515<FrameType>::bitrate() const {
    return bitrate_;
}

template <typename FrameType>
Error MCP2515<FrameType>::read(FrameType* frame) {
    if (!ready_) {
        return ERR_READY;
    }

    switch (mcp_.readMsgBufID(frame->mutable_id(), frame->mutable_size(), frame->data())) {
        case CAN_OK:
            break;
        case CAN_NOMSG:
            return ERR_FIFO;
        default:
            return ERR_INTERNAL;
    }
    frame->ext(mcp_.isExtendedFrame());
    return ERR_OK;
}

template <typename FrameType>
Error MCP2515<FrameType>::write(const FrameType& frame) {
    if (!ready_) {
        return ERR_READY;
    }
    if (frame.data() == nullptr || frame.size() > 8) {
        return ERR_INVALID;
    }

    switch(mcp_.sendMsgBuf(frame.id(), frame.ext(), frame.size(), frame.data())) {
        case CAN_OK:
            return ERR_OK;
        case CAN_GETTXBFTIMEOUT:
        case CAN_SENDMSGTIMEOUT:
            return ERR_FIFO;
        default:
            return ERR_INTERNAL;
    }
}

template <typename FrameType>
void MCP2515<FrameType>::setMask(uint8_t num, uint8_t ext, uint32_t mask) {
    mcp_.init_Mask(num, ext, mask);
}

template <typename FrameType>
void MCP2515<FrameType>::setFilter(uint8_t num, uint8_t ext, uint32_t filter) {
    mcp_.init_Filt(num, ext, filter);
}

template <typename FrameType>
void MCP2515<FrameType>::disableFilters() {
    for (uint8_t i = 0; i < 2; ++i) {
        setMask(i, 0, 0);
    }
    for (uint8_t i = 0; i < 6; ++i) {
        setFilter(i, 0, 0);
    }
}

}  // namespace Canny
