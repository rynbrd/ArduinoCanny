#include "RealDash.h"

#include <Arduino.h>
#include <CRC32.h>
#include "Frame.h"

namespace Canny {

static const uint32_t kReceiveTimeout = 5000;

RealDash::RealDash(Stream* stream) : stream_(stream), frame_(0, 1, 0, 8) {
    reset();
}

void RealDash::reset() {
    memset(checksum_buffer_, 0, 4);
    frame_type_66_ = false;
    frame44_checksum_ = 0;
    frame66_checksum_.reset();
    frame_.resize(8);
    read_size_ = 0;
}

void RealDash::updateChecksum(byte b) {
    if (frame_type_66_) {
        frame66_checksum_.update(b);
    } else {
        frame44_checksum_ += b;
    }
}

Error RealDash::read(uint32_t* id, uint8_t* ext, uint8_t* data, uint8_t* size) {
    if (stream_ && readHeader() && readId() && readData() && validateChecksum()) {
        *id = frame_.id();
        *ext = frame_.ext();
        memcpy(data, frame_.data(), frame_.size());
        *size = frame_.size();
        reset();
        return Error::ERR_OK;
    }
    return Error::ERR_FIFO;
}

Error RealDash::read(Frame* frame) {
    if (stream_ && readHeader() && readId() && readData() && validateChecksum()) {
        *frame = frame_;
        reset();
        return Error::ERR_OK;
    }
    return Error::ERR_FIFO;
}

bool RealDash::readHeader() {
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

bool RealDash::readId() {
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

bool RealDash::readData() {
    while (stream_->available() && read_size_ - 8 < frame_.size()) {
        frame_.data()[read_size_-8] = stream_->read();
        updateChecksum(frame_.data()[read_size_-8]);
        read_size_++;
    }
    *frame_.mutable_size() = frame_.size();
    return read_size_ - 8 >= frame_.size();
}

bool RealDash::validateChecksum() {
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

void RealDash::writeByte(const byte b) {
    stream_->write(b);
    write_checksum_.update(b);
}

void RealDash::writeBytes(const byte* b, uint8_t len) {
    for (int i = 0; i < len; i++) {
        writeByte(b[i]);
    }
}

void RealDash::writeBytes(uint32_t data) {
    writeBytes((const byte*)&data, 4);
}

Error RealDash::write(uint32_t id, uint8_t ext, uint8_t* data, uint8_t size) {
    if (!stream_) {
        return ERR_FIFO;
    }
    if (size > 64 || size % 4 != 0) {
        return ERR_INVALID;
    }

    write_checksum_.reset();

    byte encoded_size = (size < 8 ? 8 : size) / 4 + 15;
    writeByte(0x66);
    writeByte(0x33);
    writeByte(0x22);
    writeByte(encoded_size);
    writeBytes(id);
    if (data != nullptr) {
        writeBytes(data, size);
    }
    for (int i = 0; i < 8-size; i++) {
        writeByte(0);
    }
    uint32_t checksum = write_checksum_.value();
    stream_->write((const byte*)&checksum, 4);
    stream_->flush();
    return Error::ERR_OK;
}

Error RealDash::write(const Frame& frame) {
    return write(frame.id(), frame.ext(), frame.data(), frame.size());
}

}  // namespace Canny
