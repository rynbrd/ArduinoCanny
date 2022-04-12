#include <Arduino.h>
#include <AUnit.h>
#include <Canny/Frame.h>

using namespace aunit;
using ::Canny::Frame;

test(ConstructorTest, Default) {
    Frame f;
    
    assertEqual(f.id(), 0u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 0);
    assertEqual(f.capacity(), 0);
    assertEqual(f.data(), nullptr);
}

test(ConstructorTest, Capacity) {
    Frame f(12); 
    uint8_t expect_data[12];
    memset(expect_data, 0, 12);

    assertEqual(f.id(), 0u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 0);
    assertEqual(f.capacity(), 12);
    assertEqual(memcmp(f.data(), expect_data, 12), 0);
}

test(ConstructorTest, SetAllDefaults) {
    Frame f(0x321, 1, 8);
    uint8_t expect_data[8];
    memset(expect_data, 0, 8);

    assertEqual(f.id(), 0x321u);
    assertEqual(f.ext(), 1);
    assertEqual(f.size(), 8);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ConstructorTest, SetAllSetCapacity) {
    Frame f(0x432, 0, 8, 12);
    memset(f.data(), 0xF2, 12);
    uint8_t expect_data[12];
    memset(expect_data, 0xF2, 12);

    assertEqual(f.id(), 0x432u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual(f.capacity(), 12);
    assertEqual(memcmp(f.data(), expect_data, 12), 0);
}

test(ConstructorTest, Copy) {
    Frame f1(0x123, 0, 8);
    memset(f1.data(), 0xF1, 8);
    Frame f2(f1);

    assertEqual(f2.id(), 0x123u);
    assertEqual(f2.ext(), 0);
    assertEqual(f2.size(), 8);
    assertEqual(f2.capacity(), 8);
    assertEqual(memcmp(f2.data(), f1.data(), 8), 0);
    assertNotEqual(f1.data(), f2.data());
}

#ifdef EPOXY_DUINO
test(ConstructorTest, InitializerListDefault) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(f.capacity(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ConstructorTest, InitializerListExtraCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D}, 8);
    memset(f.data()+4, 0xFF, 4);
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0xFF, 0xFF, 0xFF, 0xFF};

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(DataTest, LessSize) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});
    uint8_t expect_data[8] = {0x5D, 0x6E, 0x80, 0x91};
    f.data({0x5D, 0x6E, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(DataTest, SameSize) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x5D, 0x6E, 0x80, 0x91};
    f.data({0x5D, 0x6E, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual(f.capacity(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(DataTest, MoreSize) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91};
    f.data({0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}
#endif

test(ReserveTest, LessCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91};
    f.reserve(4);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ReserveTest, SameCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91};
    f.reserve(8);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ReserveTest, MoreCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91};
    f.reserve(10);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual(f.capacity(), 10);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ResizeTest, LessSize) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};
    f.resize(2);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 2);
    assertEqual(f.capacity(), 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ResizeTest, MoreSizeWithinCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D}, 8);
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x00, 0x00, 0x00, 0x00};
    f.resize(6);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 6);
    assertEqual(f.capacity(), 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ResizeTest, MoreSizeExceedsCapacity) {
    Frame f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[6] = {0x1A, 0x2B, 0x4C, 0x5D, 0x00, 0x00};
    f.resize(6);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 6);
    assertEqual(f.capacity(), 6);
    assertEqual(memcmp(f.data(), expect_data, 6), 0);
}

test(ClearTest, SetFill) {
    uint8_t expect_data[64];
    memset(expect_data, 0xAA, 64);

    Frame f(0x12, 0, 32, 64);
    f.clear(0xAA);

    assertEqual(f.id(), 0x12u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 32);
    assertEqual(f.capacity(), 64);
    assertEqual(memcmp(f.data(), expect_data, 64), 0);
}

test(SetTest, SmallerFrame) {
    Frame f1(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    Frame f2(0x321, 1, {0xF1, 0xE2});
    uint8_t expect_data[2] = {0xF1, 0xE2};
    f1 = f2;

    assertEqual(f1.id(), 0x321u);
    assertEqual(f1.ext(), 1);
    assertEqual(f1.size(), 2);
    assertEqual(f1.capacity(), 4);
    assertEqual(memcmp(f1.data(), expect_data, 2), 0);
}

test(SetTest, LargerFrame) {
    Frame f1(0x321, 1, {0xF1, 0xE2});
    Frame f2(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};
    f1 = f2;

    assertEqual(f1.id(), 0x123u);
    assertEqual(f1.ext(), 0);
    assertEqual(f1.size(), 4);
    assertEqual(f1.capacity(), 4);
    assertEqual(memcmp(f1.data(), expect_data, 4), 0);
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
    Frame f1(0x01, 0, 8, 8);
    Frame f2(0x01, 0, 8, 8);
    memset(f2.data(), 0xFF, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentSize) {
    Frame f1(0x01, 0, 8, 64);
    Frame f2(0x01, 0, 16, 64);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentCapacity) {
    Frame f1(0x01, 0, 8, 8);
    Frame f2(0x01, 0, 8, 64);
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
