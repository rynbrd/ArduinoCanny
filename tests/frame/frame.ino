#include <Arduino.h>
#include <AUnit.h>
#include <Canny/Frame.h>

using namespace aunit;
using ::Canny::Frame;

test(ConstructorTest, Default) {
    Frame f;
    
    assertEqual(f.id, 0u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 0);
    assertEqual(f.capacity(), 0);
    assertEqual(f.data, nullptr);
}

test(ConstructorTest, CapacityDefaultFill) {
    Frame f(12); 
    uint8_t expect_data[12];
    memset(expect_data, 0, 12);

    assertEqual(f.id, 0u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 0);
    assertEqual(f.capacity(), 12);
    assertEqual(memcmp(f.data, expect_data, 12), 0);
}

test(ConstructorTest, CapacitySetFill) {
    Frame f(21, 0xA1); 
    uint8_t expect_data[21];
    memset(expect_data, 0xA1, 21);

    assertEqual(f.id, 0u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 0);
    assertEqual(f.capacity(), 21);
    assertEqual(memcmp(f.data, expect_data, 21), 0);
}

test(ConstructorTest, SetAllDefaults) {
    Frame f(0x321, 1, 8);
    uint8_t expect_data[8];
    memset(expect_data, 0, 8);

    assertEqual(f.id, 0x321u);
    assertEqual(f.ext, 1);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data, expect_data, 8), 0);
}

test(ConstructorTest, SetAllSetFill) {
    Frame f(0x432, 0, 8, 12, 0xF2);
    uint8_t expect_data[12];
    memset(expect_data, 0xF2, 12);

    assertEqual(f.id, 0x432u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 12);
    assertEqual(memcmp(f.data, expect_data, 12), 0);
}

test(ConstructorTest, Copy) {
    Frame f1(0x123, 0, 8, 8, 0xF1);
    Frame f2 = f1;

    assertEqual(f2.id, 0x123u);
    assertEqual(f2.ext, 0);
    assertEqual(f2.size, 8);
    assertEqual(f2.capacity(), 8);
    assertEqual(memcmp(f2.data, f1.data, 8), 0);
    assertNotEqual(f1.data, f2.data);
}

#ifdef EPOXY_DUINO
test(ConstructorTest, InitializerListDefault) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    assertEqual(f.id, 0x123u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 4);
    assertEqual(f.capacity(), 4);
    assertEqual(memcmp(f.data, expect_data, 4), 0);
}

test(ConstructorTest, InitializerListExtraCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D}, 8, 0xFF);
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0xFF, 0xFF, 0xFF, 0xFF};

    assertEqual(f.id, 0x123u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 4);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data, expect_data, 8), 0);
}
#endif

test(CopyTest, DefaultCapacity) {
    uint8_t expect_data[8];
    memset(expect_data, 0xB3, 8);
    Frame f = Frame::copy(0x567, 0, expect_data, 8);

    assertEqual(f.id, 0x567u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data, expect_data, 8), 0);
    assertNotEqual(f.data, expect_data);
}

test(CopyTest, SetCapacity) {
    uint8_t src_data[8];
    memset(src_data, 0xB4, 8);

    uint8_t expect_data[12];
    memset(expect_data, 0xB4, 8);
    memset(expect_data+8, 0x00, 4);

    Frame f = Frame::copy(0x567, 0, src_data, 8, 12);

    assertEqual(f.id, 0x567u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 12);
    assertEqual(memcmp(f.data, expect_data, 12), 0);
    assertNotEqual(f.data, expect_data);
}

test(WrapTest, DefaultCapacity) {
    uint8_t expect_data[8];
    memset(expect_data, 0xC5, 8);
    Frame f = Frame::wrap(0x987, 1, expect_data, 8);

    assertEqual(f.id, 0x987u);
    assertEqual(f.ext, 1);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 8);
    assertEqual(f.data, expect_data);
}

test(WrapTest, SetCapacity) {
    uint8_t expect_data[12];
    memset(expect_data, 0xC5, 8);
    memset(expect_data+8, 0xC5, 4);
    Frame f = Frame::wrap(0x987, 1, expect_data, 8, 12);

    assertEqual(f.id, 0x987u);
    assertEqual(f.ext, 1);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 12);
    assertEqual(f.data, expect_data);
}

test(ClearTest, DefaultFill) {
    uint8_t expect_data[64];
    memset(expect_data, 0x00, 64);

    Frame f(0x12, 0, 32, 64, 0xFF);
    f.clear();

    assertEqual(f.id, 0x12u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 32);
    assertEqual(f.capacity(), 64);
    assertEqual(memcmp(f.data, expect_data, 64), 0);
}

test(ClearTest, SetFill) {
    uint8_t expect_data[64];
    memset(expect_data, 0xAA, 64);

    Frame f(0x12, 0, 32, 64, 0xFF);
    f.clear(0xAA);

    assertEqual(f.id, 0x12u);
    assertEqual(f.ext, 0);
    assertEqual(f.size, 32);
    assertEqual(f.capacity(), 64);
    assertEqual(memcmp(f.data, expect_data, 64), 0);
}

test(SetTest, Defualts) {
    uint8_t expect_data[64];
    memset(expect_data, 0x00, 64);

    Frame f(64);
    f.set(0x13, 1);

    assertEqual(f.id, 0x13u);
    assertEqual(f.ext, 1);
    assertEqual(f.size, 0);
    assertEqual(f.capacity(), 64);
    assertEqual(memcmp(f.data, expect_data, 64), 0);
}

test(SetTest, SetSizeAndFill) {
    uint8_t expect_data[64];
    memset(expect_data, 0xFF, 64);

    Frame f(64);
    f.set(0x13, 1, 8, 0xFF);

    assertEqual(f.id, 0x13u);
    assertEqual(f.ext, 1);
    assertEqual(f.size, 8);
    assertEqual(f.capacity(), 64);
    assertEqual(memcmp(f.data, expect_data, 64), 0);
}

test(EqualsTest, SameFrame) {
    Frame f(0x01, 0, 8);
    assertTrue(f == f);
}

test(EqualsTest, DifferentID) {
    Frame f1(0x01, 0, 8);
    Frame f2(0x02, 0, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentExt) {
    Frame f1(0x01, 0, 8);
    Frame f2(0x01, 1, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentData) {
    Frame f1(0x01, 0, 8, 8, 0x00);
    Frame f2(0x01, 0, 8, 8, 0xFF);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentSize) {
    Frame f1(0x01, 0, 8, 64, 0x00);
    Frame f2(0x01, 0, 16, 64, 0x00);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentCapacity) {
    Frame f1(0x01, 0, 8, 8, 0x00);
    Frame f2(0x01, 0, 8, 64, 0x00);
    assertTrue(f1 == f2);
}

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
