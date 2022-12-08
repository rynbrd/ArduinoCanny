#ifndef _CANNY_BUFFER_H_
#define _CANNY_BUFFER_H_

#include <Arduino.h>
#include <Foundation.h>
#include "Connection.h"
#include "Frame.h"

namespace Canny {

// A CAN connection that buffers reads and writes. Supports pre-filtering of
// reads and writes to avoid filling buffers with frames that should be ignored.
template <typename FrameType>
class BufferedConnection : public Connection<FrameType> {
    public:
        // Construct a buffered connection that reads/writes to the child
        // connection. Buffers of the given sizes are created.
        BufferedConnection(
                Connection<FrameType>* child,
                size_t read_buffer_size,
                size_t write_buffer_size);

        // Read a frame and populate the buffer while frames are available to
        // read from the child connection. Always returns the first frame in
        // the buffer or reads a frame from the child when the buffer is empty.
        // Return ERR_OK on success or ERR_FIFO if there are no frames in the
        // buffer and no frame is available to be read. Other errors are logged
        // via the onReadError method.
        Error read(FrameType* frame) override;

        // Write a frame to the child. Attempts to write buffered frames before
        // writing the provided frame. Writes which receive ERR_FIFO are
        // buffered and retried on the next call to write. Write encountering
        // other errors are discarded and logged via the onWriteError method.
        //
        // Return ERR_OK when a write succeeds or is buffered. Return ERR_FIFO
        // if the write fails with ERR_FIFO and the internal buffer is full.
        Error write(const FrameType& frame) override;

        // Flush buffered writes. This should happen in loop() to avoid delays
        // when write() isn't being called frequently.
        void flush();

        // Filter frames read from the child connection. Filtered frames are
        // not buffered. Return true if a frame should be read or false to
        // filter a frame.
        virtual bool readFilter(const FrameType&) const { return true; }

        // Filter frames written to the child connection. Filtered frames are
        // not buffered. true if a frame should be written or false false to
        // filter a frame.
        virtual bool writeFilter(const FrameType&) const { return true; }

        // Called by read() when a read error occurs. Only non-FIFO errors are
        // handled by this method.
        virtual void onReadError(Error) const {}

        // Called by write() when a frame is discarded due to a write error.
        // Error is ERR_FIFO when the write buffer is full and the frame must
        // be discarded.
        virtual void onWriteError(Error, const FrameType&) const {}

    private:
        void fillReadBuffer();
        Error drainWriteBuffer();

        Connection<FrameType>* child_;
        Queue<FrameType> read_queue_;
        Queue<FrameType> write_queue_;
};

}  // namespace Canny

#include "Buffer.tpp"

#endif  // _CANNY_BUFFER_H_
