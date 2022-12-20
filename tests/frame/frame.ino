#include <Arduino.h>
#include <AUnit.h>
#include <Canny.h>

using namespace aunit;

namespace Canny {

// Variant type to help test that the frame types are trivially
// constructable/copyable and work in a union.
class FrameVariant {
    public:
        enum Type {
            CAN20,
            CANFD,
            J1939,
        };

        FrameVariant(const CAN20Frame& frame) :
            type_(CAN20), can20_(frame) {}

        FrameVariant(const CANFDFrame& frame) :
            type_(CANFD), canfd_(frame) {}

        FrameVariant(const J1939Message& message) :
            type_(J1939), j1939_(message) {}

        ~FrameVariant() {
            switch (type_) {
                case CAN20:
                    can20_.~CAN20Frame();
                    break;
                case CANFD:
                    canfd_.~CANFDFrame();
                    break;
                case J1939:
                    j1939_.~J1939Message();
                    break;
            }
        }

        Type type() { return type_; } 

        CAN20Frame* can20() {
            return type_ == CAN20 ? &can20_ : nullptr;
        }

        CANFDFrame* canfd() {
            return type_ == CANFD ? &canfd_ : nullptr;
        }

        J1939Message* j1939() {
            return type_ == J1939 ? &j1939_ : nullptr;
        }

    private:
        Type type_;
        union {
            CAN20Frame can20_;
            CANFDFrame canfd_;
            J1939Message j1939_;
        };
};

test(VariantTest, RoundTrip) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    FrameVariant v(f);
    assertEqual(v.type(), v.CAN20);
    assertTrue(v.can20() != nullptr);
    assertTrue(*v.can20() == f);
}

test(ConstructorTest, Default) {
    CAN20Frame f;
    
    assertEqual(f.id(), 0u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 0);
    assertNotEqual(f.data(), nullptr);
}

test(ConstructorTest, SetAll) {
    CAN20Frame f(0x321, 1, 0);
    uint8_t expect_data[8];
    memset(expect_data, 0, 8);

    assertEqual(f.id(), 0x321u);
    assertEqual(f.ext(), 1);
    assertEqual(f.size(), 0);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ConstructorTest, Copy) {
    CAN20Frame f1(0x123, 0, 8);
    memset(f1.data(), 0xF1, 8);
    CAN20Frame f2(f1);

    assertEqual(f2.id(), 0x123u);
    assertEqual(f2.ext(), 0);
    assertEqual(f2.size(), 8);
    assertEqual(memcmp(f2.data(), f1.data(), 8), 0);
    assertNotEqual(f1.data(), f2.data());
}

test(ConstructorTest, InitializerListDefault) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(DataTest, LessSize) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});
    uint8_t expect_data[8] = {0x5D, 0x6E, 0x80, 0x91};
    f.data((uint8_t[]){0x5D, 0x6E, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(DataTest, SameSize) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x5D, 0x6E, 0x80, 0x91};
    f.data((uint8_t[]){0x5D, 0x6E, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ResizeTest, LessSize) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x00, 0x00};
    f.resize(2);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 2);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ResizeTest, MoreSizeWithinCapacity) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x00, 0x00, 0x00, 0x00};
    f.resize(6);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 6);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ResizeTest, MoreSizeExceedsCapacity) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x00, 0x00, 0x00, 0x00};
    f.resize(10);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}


test(SetTest, SmallerFrame) {
    CANFDFrame f1(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91, 0x52, 0xBC});
    CAN20Frame f2(0x321, 1, (uint8_t[]){0xF1, 0xE2});
    uint8_t expect_data[2] = {0xF1, 0xE2};
    f1.copyFrom(f2);

    assertEqual(f1.id(), 0x321u);
    assertEqual(f1.ext(), 1);
    assertEqual(f1.size(), 2);
    assertEqual(memcmp(f1.data(), expect_data, 2), 0);
}

test(SetTest, LargerFrame) {
    CAN20Frame f1(0x321, 1, (uint8_t[]){0xF1, 0xE2});
    CANFDFrame f2(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91, 0x52, 0xBC});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91};
    f1.copyFrom(f2);

    assertEqual(f1.id(), 0x123u);
    assertEqual(f1.ext(), 0);
    assertEqual(f1.size(), 8);
    assertEqual(memcmp(f1.data(), expect_data, 2), 0);
}

test(EqualsTest, SameFrame) {
    CAN20Frame f(0x01, 0, 8);
    assertTrue(f == f);
}

test(EqualsTest, DifferentID) {
    CAN20Frame f1(0x01, 0, 8);
    CAN20Frame f2(0x02, 0, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentExt) {
    CAN20Frame f1(0x01, 0, 8);
    CAN20Frame f2(0x01, 1, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentData) {
    CAN20Frame f1(0x01, 0, 8);
    CAN20Frame f2(0x01, 0, 8);
    memset(f2.data(), 0xFF, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentSize) {
    CANFDFrame f1(0x01, 0, 8);
    CANFDFrame f2(0x01, 0, 16);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentCapacity) {
    CAN20Frame f1(0x01, 0, 8);
    CANFDFrame f2(0x01, 0, 8);
    assertTrue(f1 == f2);
}

test(MutableTest, MutableID) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    *f.mutable_id() = 0x01;
    assertEqual(f.id(), 0x01u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(MutableTest, MutableExt) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    *f.mutable_ext() = 1;
    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 1);
    assertEqual(f.size(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(MutableTest, MutableSize) {
    CAN20Frame f(0x123, 0, (uint8_t[]){0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    *f.mutable_size() = 2;
    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 2);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(MutableTest, MutableData) {
    CAN20Frame f(0x123, 0, 4);
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};
    memcpy(f.data(), expect_data, 4);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
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
