#include "Buffer.h"

#include "Arduino.h"
#include "Queue.h"

namespace Canny {

Error BufferedConnection::read(Frame* frame) {
    if (!read_queue_.empty()) {
        Serial.println("read: buffer available");
        *frame = *read_queue_.dequeue();
    } else {
        Serial.println("read: buffer empty");
        Error err;
        do {
            err = child_->read(frame);
            if (err != ERR_OK) {
                if (err != ERR_FIFO) {
                    onReadError(err);
                    Serial.println("read: error");
                } else {
                    Serial.println("read: nothing to read");
                }
                return ERR_FIFO;
            }
            if (!readFilter(*frame)) {
                Serial.print("read: filter frame: "); Serial.println(*frame);
            } 
        } while(!readFilter(*frame));
    }
    Serial.print("read: got frame: "); Serial.println(*frame);
    fillReadBuffer();
    return ERR_OK;
}

Error BufferedConnection::write(const Frame& frame) {
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

void BufferedConnection::flush() {
    drainWriteBuffer();
} 

void BufferedConnection::fillReadBuffer() {
    Frame* frame;
    Error err;
    Serial.println("buffer: reading");
    while ((frame = read_queue_.alloc()) != nullptr) {
        Serial.println("buffer: pointer allocated");
        err = child_->read(frame);
        if (err == ERR_FIFO) {
            Serial.println("  nothing to read");
            break;
        } else if (err != ERR_OK) {
            Serial.println("  read error");
            onReadError(err);
            break;
        }
        if (readFilter(*frame)) {
            Serial.print("  buffered frame: ");Serial.println(*frame);
            read_queue_.enqueue(*frame);
        } else {
            Serial.print("  filtered frame: ");Serial.println(*frame);
        }
    }
}

Error BufferedConnection::drainWriteBuffer() {
    Frame* frame;
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
