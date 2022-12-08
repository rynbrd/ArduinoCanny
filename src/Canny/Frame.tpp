namespace Canny {

template <size_t Capacity>
Frame<Capacity>::Frame() : id_(0), ext_(0), size_(0), pad_(0x00) {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity>
Frame<Capacity>::Frame(uint8_t size, uint8_t pad) :
        id_(0), ext_(0), size_(size), pad_(pad) {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity>
Frame<Capacity>::Frame(uint32_t id, uint8_t ext, uint8_t size, uint8_t pad) :
        id_(id), ext_(ext), size_(size), pad_(pad) {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity>
template <size_t N> 
Frame<Capacity>::Frame(uint32_t id, uint8_t ext, const uint8_t (&data)[N], uint8_t pad) :
        id_(id), ext_(ext), size_(min(Capacity, N)), pad_(pad) {
    for (size_t i = 0; i < size_; i++) {
        data_[i] = data[i];
    }
    memset(data_+size_, pad_, Capacity-size_);
}

template <size_t Capacity>
void Frame<Capacity>::id(uint32_t id, uint8_t ext) {
    id_ = id;
    ext_ = (ext == 1) ? 1 : 0;
}

template <size_t Capacity>
void Frame<Capacity>::data(const uint8_t* data, uint8_t len) {
    if (len > Capacity) {
        len = Capacity;
    }
    resize(len);
    memcpy(data_, data, len);
}

template <size_t Capacity>
template <size_t N> 
void Frame<Capacity>::data(const uint8_t (&data)[N]) {
    resize(sizeof(data));
    for (size_t i = 0; i < sizeof(data); i++) {
        data_[i] = data[i];
    }
}

template <size_t Capacity>
void Frame<Capacity>::resize(uint8_t size) {
    if (size > Capacity) {
        size = Capacity;
    } else if (size < Capacity) {
        memset(data_+size, pad_, Capacity-size);
    }
    size_ = size;
}

template <size_t Capacity>
void Frame<Capacity>::clear() {
    memset(data_, pad_, Capacity);
}

template <size_t Capacity>
void Frame<Capacity>::clear(uint8_t pad) {
    pad_ = pad;
    clear();
}

template <size_t Capacity>
size_t Frame<Capacity>::printTo(Print& p) const {
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

template <size_t Capacity>
template <size_t OtherCapacity>
Frame<Capacity>& Frame<Capacity>::operator=(const Frame<OtherCapacity>& other) {
    id_ = other.id();
    ext_ = other.ext();
    if (Capacity < other.size()) {
        size_ = Capacity;
    } else {
        size_ = other.size();
    }
    memcpy(data_, other.data(), size_);
    return *this;
}

template <size_t LeftCapacity, size_t RightCapacity>
bool operator==(const Frame<LeftCapacity>& left, const Frame<RightCapacity>& right) {
    if (left.id() != right.id() || left.size() != right.size() || left.ext() != right.ext()) {
        return false;
    }
    if (left.data() == right.data()) {
        return true;
    }
    return memcmp(left.data(), right.data(), left.size()) == 0;
}

template <size_t LeftCapacity, size_t RightCapacity>
bool operator!=(const Frame<LeftCapacity>& left, const Frame<RightCapacity>& right) {
    return !(left == right);
}

}  // namespace Canny
