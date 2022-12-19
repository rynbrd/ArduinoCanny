namespace Canny {

template <size_t Capacity, uint8_t Pad>
Frame<Capacity, Pad>::Frame() : id_(0), ext_(0), size_(0) {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity, uint8_t Pad>
Frame<Capacity, Pad>::Frame(uint32_t id, uint8_t ext, uint8_t size) :
        id_(id), ext_(ext), size_(size) {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity, uint8_t Pad>
void Frame<Capacity, Pad>::id(uint32_t id, uint8_t ext) {
    id_ = id;
    ext_ = (ext == 1) ? 1 : 0;
}

template <size_t Capacity, uint8_t Pad>
void Frame<Capacity, Pad>::data(const uint8_t* data, uint8_t len) {
    if (len > Capacity) {
        len = Capacity;
    }
    resize(len);
    memcpy(data_, data, len);
}

template <size_t Capacity, uint8_t Pad>
template <size_t N> 
void Frame<Capacity, Pad>::data(const uint8_t (&data)[N]) {
    resize(sizeof(data));
    for (size_t i = 0; i < sizeof(data); i++) {
        data_[i] = data[i];
    }
}

template <size_t Capacity, uint8_t Pad>
void Frame<Capacity, Pad>::resize(uint8_t size) {
    if (size > Capacity) {
        size = Capacity;
    } else if (size < Capacity) {
        memset(data_+size, pad_, Capacity-size);
    }
    size_ = size;
}

template <size_t Capacity, uint8_t Pad>
void Frame<Capacity, Pad>::clear() {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity, uint8_t Pad>
void Frame<Capacity, Pad>::clear(uint8_t fill) {
    memset(data_, fill, Capacity);
}

template <size_t Capacity, uint8_t Pad>
size_t Frame<Capacity, Pad>::printTo(Print& p) const {
    size_t n = 0;
    if (ext()) {
        n += p.print("+");
    } else {
        n += p.print("-");
    }
    n += p.print(id(), HEX);
    n += p.print("#");
    for (int i = 0; i < size(); i++) {
        if (data()[i] <= 0x0F) {
            n += p.print("0");
        }
        n += p.print(data()[i], HEX);
        if (i < size()-1) {
            n += p.print(":");
        }
    }
    return n;
}

template <size_t Capacity, uint8_t Pad>
template <size_t OtherCapacity, uint8_t OtherPad>
void Frame<Capacity, Pad>::copyFrom(const Frame<OtherCapacity, OtherPad>& other) {
    id_ = other.id();
    ext_ = other.ext();
    if (Capacity < other.size()) {
        size_ = Capacity;
    } else {
        size_ = other.size();
    }
    memcpy(data_, other.data(), size_);
}

template <size_t N> 
CAN20Frame::CAN20Frame(uint32_t id, uint8_t ext, const uint8_t (&buf)[N]) :
        Frame(id, ext, 0) {
    if (N < capacity()) {
        resize(N);
    } else {
        resize(capacity());
    }
    for (size_t i = 0; i < size(); i++) {
        data()[i] = buf[i];
    }
    memset(data()+size(), pad(), capacity()-size());
}

template <size_t N> 
CANFDFrame::CANFDFrame(uint32_t id, uint8_t ext, const uint8_t (&buf)[N]) :
        Frame(id, ext, 0) {
    if (N < capacity()) {
        resize(N);
    } else {
        resize(capacity());
    }
    for (size_t i = 0; i < size(); i++) {
        data()[i] = buf[i];
    }
    memset(data()+size(), pad(), capacity()-size());
}

template <size_t LeftCapacity, uint8_t LeftPad, size_t RightCapacity, uint8_t RightPad>
bool operator==(const Frame<LeftCapacity, LeftPad>& left, const Frame<RightCapacity, RightPad>& right) {
    if (left.id() != right.id() || left.size() != right.size() || left.ext() != right.ext()) {
        return false;
    }
    if (left.data() == right.data()) {
        return true;
    }
    return memcmp(left.data(), right.data(), left.size()) == 0;
}

template <size_t LeftCapacity, uint8_t LeftPad, size_t RightCapacity, uint8_t RightPad>
bool operator!=(const Frame<LeftCapacity, LeftPad>& left, const Frame<RightCapacity, RightPad>& right) {
    return !(left == right);
}

}  // namespace Canny
