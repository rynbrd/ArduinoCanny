namespace Canny {

static const uint32_t kReceiveTimeout = 5000;

template <typename FrameType>
RealDash<FrameType>::RealDash(Stream* stream) : stream_(stream), frame_(0, 1, 0) {
    reset();
}

template <typename FrameType>
void RealDash<FrameType>::reset() {
    memset(checksum_buffer_, 0, 4);
    frame_type_66_ = false;
    frame44_checksum_ = 0;
    frame66_checksum_.reset();
    frame_.resize(8);
    read_size_ = 0;
}

template <typename FrameType>
void RealDash<FrameType>::updateChecksum(byte b) {
    if (frame_type_66_) {
        frame66_checksum_.update(b);
    } else {
        frame44_checksum_ += b;
    }
}

template <typename FrameType>
Error RealDash<FrameType>::read(FrameType* frame) {
    if (stream_ && readHeader() && readId() && readData() && validateChecksum()) {
        *frame = frame_;
        reset();
        return Error::ERR_OK;
    }
    return Error::ERR_FIFO;
}

template <typename FrameType>
bool RealDash<FrameType>::readHeader() {
    byte b;
    while (stream_->available() && read_size_ < 4) {
        b = stream_->read();
        read_size_++;
        switch (read_size_) {
            case 1:
                if (b != 0x44 && b != 0x66) {
                    //ERROR_MSG_VAL_FMT("realdash: unrecognized frame type ", b, HEX);
                    reset();
                    continue;
                }
                frame_type_66_ = (b == 0x66);
                break;
            case 2:
                if (b != 0x33) {
                    //ERROR_MSG_VAL_FMT("realdash: invalid header byte 2 ", b, HEX);
                    reset();
                    continue;
                }
                break;
            case 3:
                if (b != 0x22) {
                    //ERROR_MSG_VAL_FMT("realdash: invalid header byte 3 ", b, HEX);
                    reset();
                    continue;
                }
                break;
            case 4:
                if ((!frame_type_66_ && b != 0x11) || (frame_type_66_ && (b < 0x11 || b > 0x1F))) {
                    //ERROR_MSG_VAL_FMT("realdash: invalid header byte 4 ", b, HEX);
                    reset();
                    continue;
                }
                frame_.resize((b - 15) * 4);
                break;
            default:
                break;
        }
        updateChecksum(b);
    }
    return read_size_ >= 4;
}

template <typename FrameType>
bool RealDash<FrameType>::readId() {
    uint32_t b;
    while (stream_->available() && read_size_ < 8) {
        b = stream_->read();
        read_size_++;
        updateChecksum(b);
        switch (read_size_-4) {
            case 1:
                *frame_.mutable_id() = b;
                break;
            case 2:
                *frame_.mutable_id() |= (b << 8);
                break;
            case 3:
                *frame_.mutable_id() |= (b << 16);
                break;
            case 4:
                *frame_.mutable_id() |= (b << 24);
                break;
        }
    }
    return read_size_ >= 8;
}

template <typename FrameType>
bool RealDash<FrameType>::readData() {
    while (stream_->available() && read_size_ - 8 < frame_.size()) {
        frame_.data()[read_size_-8] = stream_->read();
        updateChecksum(frame_.data()[read_size_-8]);
        read_size_++;
    }
    *frame_.mutable_size() = frame_.size();
    return read_size_ - 8 >= frame_.size();
}

template <typename FrameType>
bool RealDash<FrameType>::validateChecksum() {
    if (frame_type_66_) {
        while (stream_->available() && read_size_ - 8 - frame_.size() < 4) {
            checksum_buffer_[read_size_ - 8 - frame_.size()] = stream_->read();
            read_size_++;
        }
        if (read_size_ - 8 - frame_.size() < 4) {
            return false;
        }
        if (frame66_checksum_.value() != *((uint32_t*)checksum_buffer_)) {
            //ERROR_MSG_VAL_FMT("realdash: frame 0x66 checksum error, wanted ", frame66_checksum_.value(), HEX);
            reset();
            return false;
        }
    } else {
        if (read_size_ - 8 - frame_.size() < 1) {
            if (!stream_->available()) {
                return false;
            }
            checksum_buffer_[0] = stream_->read();
            read_size_++;
        }
        if (frame44_checksum_ != checksum_buffer_[0]) {
            //ERROR_MSG_VAL_FMT("realdash: frame 0x44 checksum error, wanted ", frame44_checksum_, HEX);
            reset();
            return false;
        }
    }
    return true;
}

template <typename FrameType>
void RealDash<FrameType>::writeByte(const byte b) {
    stream_->write(b);
    write_checksum_.update(b);
}

template <typename FrameType>
void RealDash<FrameType>::writeBytes(const byte* b, uint8_t len) {
    for (int i = 0; i < len; i++) {
        writeByte(b[i]);
    }
}

template <typename FrameType>
void RealDash<FrameType>::writeBytes(uint32_t data) {
    writeBytes((const byte*)&data, 4);
}

template <typename FrameType>
Error RealDash<FrameType>::write(const FrameType& frame) {
    if (!stream_) {
        return ERR_FIFO;
    }
    if (frame.size() > 64 || frame.size() % 4 != 0) {
        return ERR_INVALID;
    }

    write_checksum_.reset();

    byte encoded_size = (frame.size() < 8 ? 8 : frame.size()) / 4 + 15;
    writeByte(0x66);
    writeByte(0x33);
    writeByte(0x22);
    writeByte(encoded_size);
    writeBytes(frame.id());
    writeBytes(frame.data(), frame.size());
    for (int i = 0; i < 8-frame.size(); i++) {
        writeByte(0);
    }
    uint32_t checksum = write_checksum_.value();
    stream_->write((const byte*)&checksum, 4);
    return Error::ERR_OK;
}

}  // namespace Canny
