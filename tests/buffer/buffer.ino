#include <Arduino.h>
#include <AUnit.h>
#include <Canny.h>

using namespace aunit;

namespace Canny {

class FakeConnection : public Connection {
    public:
        FakeConnection(size_t read_size, size_t write_size) :
                read_buffer_(nullptr), read_size_(read_size), read_len_(0), read_pos_(0),
                write_buffer_(nullptr), write_size_(write_size), write_len_(0) {
            if (read_size_ > 0) {
                read_buffer_ = new Frame[read_size_];
            }
            if (write_size_ > 0) {
                write_buffer_ = new Frame[write_size_];
            }
        }

        ~FakeConnection() {
            if (read_buffer_ != nullptr) {
                delete[] read_buffer_;
            }
            if (write_buffer_ != nullptr) {
                delete[] write_buffer_;
            }
        }

        Error read(Frame* frame) override {
            if (read_pos_ >= read_len_) {
                return ERR_FIFO;
            }
            *frame = read_buffer_[read_pos_];
            ++read_pos_;
            return ERR_OK; 
        }

        Error write(const Frame& frame) override {
            if (write_len_ >= write_size_) {
                return ERR_FIFO;
            }
            write_buffer_[write_len_++] = frame;
            return ERR_OK;
        }

        template <size_t N>
        void setReadBuffer(const Frame (&frames)[N]) {
            for (size_t i = 0; i < N && i < read_size_; ++i) {
                read_buffer_[i] = frames[i];
            }
            read_len_ = N > read_size_ ? read_size_ : N;
            read_pos_ = 0;
        }

        int readsRemaining() {
            return read_len_ - read_pos_;
        }

        Frame* writeData() { return write_buffer_; }

        int writeCount() { return write_len_; }

        void writeReset(int size = -1) {
            if (size >= 0) {
                write_size_ = size;
                if (write_buffer_ != nullptr) {
                    delete[] write_buffer_;
                }
                if (size > 0) {
                    write_buffer_ = new Frame[size];
                } else {
                    write_buffer_ = nullptr;
                }
            }
            write_len_ = 0;
        }

    private:
        Frame* read_buffer_;
        size_t read_size_;
        size_t read_len_;
        size_t read_pos_;
        Frame* write_buffer_;
        size_t write_size_;
        size_t write_len_;
};

class TestConnection : public BufferedConnection {
    public:
        TestConnection(Connection* child, size_t read, size_t write) :
                BufferedConnection(child, read, write, 8) {}

        bool readFilter(const Frame& frame) const override {
            return frame.id() != 0x20;
        }

        bool writeFilter(const Frame& frame) const override {
            return frame.id() != 0x20;
        }

        void onReadError(Error err) {
            Serial.print("read error: ");
            Serial.println(err);
        }

        void onWriteError(Error err, const Frame& frame) {
            Serial.print("write error: ");
            Serial.println(err);
            Serial.print("discard frame: ");
            Serial.println(frame);
        }
};

test(BufferedConnectionTest, DirectRead) {
    FakeConnection fake(1, 0);
    TestConnection can(&fake, 1, 1);

    Frame expect(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    fake.setReadBuffer({expect});

    Frame actual;
    Error err = can.read(&actual);
    assertEqual(err, Error::ERR_OK);
    assertTrue(actual == expect);
    assertEqual(fake.readsRemaining(), 0);
}

test(BufferedConnectionTest, BufferedRead) {
    FakeConnection fake(3, 0);
    TestConnection can(&fake, 3, 1);

    Frame f1(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame f2(0x11, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame f3(0x12, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    fake.setReadBuffer({f1, f2, f3});

    Frame actual;
    Error err;

    err = can.read(&actual);
    assertEqual(err, Error::ERR_OK);
    assertTrue(actual == f1);
    assertEqual(fake.readsRemaining(), 0);

    err = can.read(&actual);
    assertEqual(err, Error::ERR_OK);
    assertTrue(actual == f2);

    err = can.read(&actual);
    assertEqual(err, Error::ERR_OK);
    assertTrue(actual == f3);
}

test(BufferedConnectionTest, TestFilterChildRead) {
    FakeConnection fake(1, 0);
    TestConnection can(&fake, 1, 1);

    Frame expect(0x20, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    fake.setReadBuffer({expect});

    Frame actual;
    Error err = can.read(&actual);
    assertEqual(err, Error::ERR_FIFO);
    assertEqual(fake.readsRemaining(), 0);
}

test(BufferedConnectionTest, DirectWrite) {
    FakeConnection fake(0, 1);
    TestConnection can(&fake, 1, 1);

    Frame expect(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});

    Error err = can.write(expect);
    assertEqual(err, Error::ERR_OK);
    assertEqual(fake.writeCount(), 1);
    assertTrue(fake.writeData()[0] == expect);
}

test(BufferedConnectionTest, BufferedWrite) {
    FakeConnection fake(0, 0);
    TestConnection can(&fake, 1, 1);

    Frame expect1(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame expect2(0x11, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});

    Error err;

    err = can.write(expect1);
    assertEqual(err, Error::ERR_OK);

    fake.writeReset(2);
    err = can.write(expect2);
    assertEqual(fake.writeCount(), 2);
    assertTrue(fake.writeData()[0] == expect1);
    assertTrue(fake.writeData()[1] == expect2);
}

test(BufferedConnectionTest, FilteredWrite) {
    FakeConnection fake(0, 0);
    TestConnection can(&fake, 1, 2);

    Frame expect1(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame discard(0x20, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame expect2(0x11, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});

    Error err;

    err = can.write(expect1);
    assertEqual(err, Error::ERR_OK);
    err = can.write(discard);
    assertEqual(err, Error::ERR_OK);
    err = can.write(expect2);
    assertEqual(err, Error::ERR_OK);

    fake.writeReset(3);
    can.flush();
    assertEqual(fake.writeCount(), 2);
    assertTrue(fake.writeData()[0] == expect1);
    assertTrue(fake.writeData()[1] == expect2);
}

}  // namespace Canny

// Test boilerplate.
void setup() {
#ifdef ARDUINO
    delay(1000);
#endif
    SERIAL_PORT_MONITOR.begin(115200);
    while(!SERIAL_PORT_MONITOR);
}

void loop() {
    TestRunner::run();
    delay(1);
}
