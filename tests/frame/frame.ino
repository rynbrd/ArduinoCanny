#include <Arduino.h>
#include <AUnit.h>
#include <Canny.h>

using namespace aunit;

namespace Canny {

test(ConstructorTest, Default) {
    Frame<8> f;
    
    assertEqual(f.id(), 0u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 0);
    assertEqual((int)f.capacity, 8);
    assertNotEqual(f.data(), nullptr);
}

test(ConstructorTest, SetAllDefaults) {
    Frame<8> f(0x321, 1, 0);
    uint8_t expect_data[8];
    memset(expect_data, 0, 8);

    assertEqual(f.id(), 0x321u);
    assertEqual(f.ext(), 1);
    assertEqual(f.size(), 0);
    assertEqual((int)f.capacity, 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ConstructorTest, SetAllPad) {
    Frame<12> f(0x432, 0, 8, 0xF2);
    uint8_t expect_data[12];
    memset(expect_data, 0xF2, 12);

    assertEqual(f.id(), 0x432u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 8);
    assertEqual((int)f.capacity, 12);
    assertEqual(memcmp(f.data(), expect_data, 12), 0);
}

test(ConstructorTest, Copy) {
    Frame<8> f1(0x123, 0, 8);
    memset(f1.data(), 0xF1, 8);
    Frame<8> f2(f1);

    assertEqual(f2.id(), 0x123u);
    assertEqual(f2.ext(), 0);
    assertEqual(f2.size(), 8);
    assertEqual((int)f2.capacity, 8);
    assertEqual(memcmp(f2.data(), f1.data(), 8), 0);
    assertNotEqual(f1.data(), f2.data());
}

test(ConstructorTest, InitializerListDefault) {
    Frame<8> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 8);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(DataTest, LessSize) {
    Frame<8> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D, 0x6E, 0x7F, 0x80, 0x91});
    uint8_t expect_data[8] = {0x5D, 0x6E, 0x80, 0x91};
    f.data({0x5D, 0x6E, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 8);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(DataTest, SameSize) {
    Frame<4> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x5D, 0x6E, 0x80, 0x91};
    f.data({0x5D, 0x6E, 0x80, 0x91});

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ResizeTest, LessSize) {
    Frame<4> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x00, 0x00};
    f.resize(2);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 2);
    assertEqual((int)f.capacity, 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ResizeTest, MoreSizeWithinCapacity) {
    Frame<8> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[8] = {0x1A, 0x2B, 0x4C, 0x5D, 0x00, 0x00, 0x00, 0x00};
    f.resize(6);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 6);
    assertEqual((int)f.capacity, 8);
    assertEqual(memcmp(f.data(), expect_data, 8), 0);
}

test(ResizeTest, MoreSizeExceedsCapacity) {
    Frame<4> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};
    f.resize(6);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(ClearTest, SetPad) {
    uint8_t expect_data[64];
    memset(expect_data, 0xAA, 64);

    Frame<64> f(0x12, 0, 32);
    f.clear(0xAA);

    assertEqual(f.id(), 0x12u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 32);
    assertEqual((int)f.capacity, 64);
    assertEqual(memcmp(f.data(), expect_data, 64), 0);
}

test(SetTest, SmallerFrame) {
    Frame<4> f1(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    Frame<2> f2(0x321, 1, {0xF1, 0xE2});
    uint8_t expect_data[2] = {0xF1, 0xE2};
    f1 = f2;

    assertEqual(f1.id(), 0x321u);
    assertEqual(f1.ext(), 1);
    assertEqual(f1.size(), 2);
    assertEqual((int)f1.capacity, 4);
    assertEqual(memcmp(f1.data(), expect_data, 2), 0);
}

test(SetTest, LargerFrame) {
    Frame<2> f1(0x321, 1, {0xF1, 0xE2});
    Frame<4> f2(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B};
    f1 = f2;

    assertEqual(f1.id(), 0x123u);
    assertEqual(f1.ext(), 0);
    assertEqual(f1.size(), 2);
    assertEqual((int)f1.capacity, 2);
    assertEqual(memcmp(f1.data(), expect_data, 2), 0);
}

test(EqualsTest, SameFrame) {
    Frame<8> f(0x01, 0, 8);
    assertTrue(f == f);
}

test(EqualsTest, DifferentID) {
    Frame<8> f1(0x01, 0, 8);
    Frame<8> f2(0x02, 0, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentExt) {
    Frame<8> f1(0x01, 0, 8);
    Frame<8> f2(0x01, 1, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentData) {
    Frame<8> f1(0x01, 0, 8);
    Frame<8> f2(0x01, 0, 8);
    memset(f2.data(), 0xFF, 8);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentSize) {
    Frame<64> f1(0x01, 0, 8);
    Frame<64> f2(0x01, 0, 16);
    assertTrue(f1 != f2);
}

test(EqualsTest, DifferentCapacity) {
    Frame<8> f1(0x01, 0, 8);
    Frame<64> f2(0x01, 0, 8);
    assertTrue(f1 == f2);
}

test(MutableTest, MutableID) {
    Frame<4> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    *f.mutable_id() = 0x01;
    assertEqual(f.id(), 0x01u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(MutableTest, MutableExt) {
    Frame<4> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    *f.mutable_ext() = 1;
    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 1);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(MutableTest, MutableSize) {
    Frame<4> f(0x123, 0, {0x1A, 0x2B, 0x4C, 0x5D});
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};

    *f.mutable_size() = 2;
    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 2);
    assertEqual((int)f.capacity, 4);
    assertEqual(memcmp(f.data(), expect_data, 4), 0);
}

test(MutableTest, MutableData) {
    Frame<4> f(0x123, 0, 4);
    uint8_t expect_data[4] = {0x1A, 0x2B, 0x4C, 0x5D};
    memcpy(f.data(), expect_data, 4);

    assertEqual(f.id(), 0x123u);
    assertEqual(f.ext(), 0);
    assertEqual(f.size(), 4);
    assertEqual((int)f.capacity, 4);
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
