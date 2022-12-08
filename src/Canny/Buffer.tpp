namespace Canny {

template <typename FrameType>
BufferedConnection<FrameType>::BufferedConnection(
        Connection<FrameType>* child,
        size_t read_buffer_size,
        size_t write_buffer_size) :
    child_(child),
    read_queue_(read_buffer_size),
    write_queue_(write_buffer_size) {}

template <typename FrameType>
Error BufferedConnection<FrameType>::read(FrameType* frame) {
    if (!read_queue_.empty()) {
        *frame = *read_queue_.dequeue();
    } else {
        Error err;
        do {
            err = child_->read(frame);
            if (err != ERR_OK) {
                if (err != ERR_FIFO) {
                    onReadError(err);
                }
                return ERR_FIFO;
            }
        } while(!readFilter(*frame));
    }
    fillReadBuffer();
    return ERR_OK;
}

template <typename FrameType>
Error BufferedConnection<FrameType>::write(const FrameType& frame) {
    // write buffered frames
    Error err = drainWriteBuffer();
    if (err != ERR_OK) {
        // drain failed, queue this frame for later if it passes the filter
        if (writeFilter(frame) && !write_queue_.enqueue(frame)) {
            // no room in buffer, discard frame
            onWriteError(ERR_FIFO, frame);
            return ERR_FIFO;
        }
        return ERR_OK;
    }

    // filter written frames
    if (!writeFilter(frame)) {
        return ERR_OK;
    }

    // write this frame
    err = child_->write(frame);
    if (err == ERR_FIFO) {
        // write failed, queue this frame for later
        if (!write_queue_.enqueue(frame)) {
            // no room in buffer, discard frame
            onWriteError(ERR_FIFO, frame);
            return ERR_FIFO;
        }
        return ERR_OK;
    } else if (err != ERR_OK) {
        // treat all non-FIFO the errors the same; log and ignore
        onWriteError(err, frame);
    }
    return ERR_OK;
}

template <typename FrameType>
void BufferedConnection<FrameType>::flush() {
    drainWriteBuffer();
} 

template <typename FrameType>
void BufferedConnection<FrameType>::fillReadBuffer() {
    FrameType* frame;
    Error err;
    while ((frame = read_queue_.alloc()) != nullptr) {
        err = child_->read(frame);
        if (err == ERR_FIFO) {
            break;
        } else if (err != ERR_OK) {
            onReadError(err);
            break;
        }
        if (readFilter(*frame)) {
            read_queue_.enqueue(*frame);
        }
    }
}

template <typename FrameType>
Error BufferedConnection<FrameType>::drainWriteBuffer() {
    FrameType* frame;
    Error err;
    while ((frame = write_queue_.peek()) != nullptr) {
        err = child_->write(*frame);
        if (err == ERR_FIFO) {
            // try again later
            return ERR_FIFO;
        } else if (err != ERR_OK) {
            // unrecoverable error, discard write
            onWriteError(err, *frame);
        }
        write_queue_.dequeue();
    }
    return ERR_OK;
}

}  // namespace Canny
