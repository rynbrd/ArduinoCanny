namespace Canny {

template <typename Frame>
Queue<Frame>::Queue() : Queue(0) {}

template <typename Frame>
Queue<Frame>::Queue(size_t capacity, size_t frame_reserve_capacity) :
        buffer_(nullptr), capacity_(capacity), size_(0), front_(0) {
    if (capacity_ > 0) {
        buffer_ = new Frame[capacity_];
        if (frame_reserve_capacity > 0) {
            for (size_t i = 0; i < capacity_; ++i) {
                buffer_[i].reserve(frame_reserve_capacity);
            }
        }
    }
}

template <typename Frame>
Queue<Frame>::~Queue() {
    if (buffer_ != nullptr) {
        delete[] buffer_;
    }
}

template <typename Frame>
Frame* Queue<Frame>::alloc() {
    if (full()) {
        return nullptr;
    }
    return buffer_ + ((front_ + size_) % capacity_);
}

template <typename Frame>
bool Queue<Frame>::enqueue(const Frame& frame) {
    if (full()) {
        return false;
    }
    size_t i = (front_ + size_) % capacity_;
    if (&frame != buffer_ + i) {
        buffer_[i] = frame;
    }
    ++size_;
    return true;
}

template <typename Frame>
Frame* Queue<Frame>::dequeue() {
    Frame* frame = peek();
    if (frame != nullptr) {
        front_ = (front_ + 1) % capacity_;
        --size_;
    }
    return frame;
}

template <typename Frame>
Frame* Queue<Frame>::peek() const {
    if (empty()) {
        return nullptr;
    }
    return buffer_ + front_;
}

}
