#include <AUnit.h>
#include <Arduino.h>
#include <Canny/Filter.h>

using namespace aunit;

namespace Canny {

test(FrameIDFilterTest, AllowEmpty) {
    FrameIDFilter filter(FilterMode::ALLOW);

    assertTrue(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));
    assertTrue(filter.match(0x04121388));
    assertTrue(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, AllowWithDropOne) {
    FrameIDFilter filter(FilterMode::ALLOW);
    filter.drop(0x00005432);

    assertTrue(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));
    assertTrue(filter.match(0x04121388));
    assertTrue(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, AllowWithDropMultiple) {
    FrameIDFilter filter(FilterMode::ALLOW);
    filter.drop(0x00005432);
    filter.drop(0x04121388);

    assertTrue(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));
    assertFalse(filter.match(0x04121388));
    assertTrue(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, AllowWithDropAndAllow) {
    FrameIDFilter filter(FilterMode::ALLOW);
    filter.drop(0x00005432);
    filter.drop(0x04121388);
    filter.allow(0x00005432);

    assertTrue(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));
    assertFalse(filter.match(0x04121388));
    assertTrue(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, AllowWithDropTwiceAndAllowOnce) {
    FrameIDFilter filter(FilterMode::ALLOW);
    filter.drop(0x00005432);
    filter.drop(0x00005432);
    filter.drop(0x04121388);
    filter.allow(0x00005432);

    assertTrue(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));
    assertFalse(filter.match(0x04121388));
    assertTrue(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, DropEmpty) {
    FrameIDFilter filter(FilterMode::DROP);

    assertFalse(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));
    assertFalse(filter.match(0x04121388));
    assertFalse(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, DropWithAllowOne) {
    FrameIDFilter filter(FilterMode::DROP);
    filter.allow(0x00005432);

    assertFalse(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));
    assertFalse(filter.match(0x04121388));
    assertFalse(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, DropWithAllowMultiple) {
    FrameIDFilter filter(FilterMode::DROP);
    filter.allow(0x00005432);
    filter.allow(0x04121388);

    assertFalse(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));
    assertTrue(filter.match(0x04121388));
    assertFalse(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, DropWithAllowAndDrop) {
    FrameIDFilter filter(FilterMode::DROP);
    filter.allow(0x00005432);
    filter.allow(0x04121388);
    filter.drop(0x00005432);

    assertFalse(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));
    assertTrue(filter.match(0x04121388));
    assertFalse(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, DropWithAlloTwiceAndDropOnce) {
    FrameIDFilter filter(FilterMode::DROP);
    filter.allow(0x00005432);
    filter.allow(0x00005432);
    filter.allow(0x04121388);
    filter.drop(0x00005432);

    assertFalse(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));
    assertTrue(filter.match(0x04121388));
    assertFalse(filter.match(0xFFFFFFFF));
}

test(FrameIDFilterTest, ModeFromAllowToDrop) {
    FrameIDFilter filter(FilterMode::ALLOW);

    filter.drop(0x00005432);
    assertTrue(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));

    filter.mode(FilterMode::DROP);
    assertFalse(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));

    filter.allow(0x00000000);
    assertTrue(filter.match(0x00000000));
    assertFalse(filter.match(0x00005432));
}

test(FrameIDFilterTest, ModeFromDropToAllow) {
    FrameIDFilter filter(FilterMode::DROP);

    filter.allow(0x00005432);
    assertFalse(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));

    filter.mode(FilterMode::ALLOW);
    assertTrue(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));

    filter.drop(0x00000000);
    assertFalse(filter.match(0x00000000));
    assertTrue(filter.match(0x00005432));
}

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
