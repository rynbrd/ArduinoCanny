#include <Arduino.h>
#include <AUnit.h>
#include <Canny/J1939.h>

using namespace aunit;

namespace Canny {

test(QueueTest, ZeroCapacity) {
    Frame expect(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Queue<Frame> q;
    assertEqual(q.enqueue(expect), false);
    assertEqual(q.dequeue(), nullptr);
    assertEqual(q.peek(), nullptr);
    assertEqual(q.size(),  (size_t)0);
    assertEqual(q.capacity(), (size_t)0);
    assertEqual(q.empty(), true);
    assertEqual(q.full(), true);
    assertEqual(q.alloc(), nullptr);
}

test(QueueTest, ExtraCapacity) {
    Frame expect(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});

    Queue<Frame> q(2);
    assertEqual(q.capacity(), (size_t)2);
    assertEqual(q.empty(), true);
    assertEqual(q.peek(), nullptr);

    assertEqual(q.enqueue(expect), true);
    assertEqual(q.size(),  (size_t)1);
    assertEqual(q.empty(), false);
    assertEqual(q.full(), false);

    assertTrue(*q.peek() == expect);
    assertTrue(*q.dequeue() == expect);

    assertEqual(q.peek(), nullptr);
    assertEqual(q.dequeue(), nullptr);
    assertEqual(q.empty(), true);
    assertEqual(q.full(), false);
}

test(QueueTest, NotEnoughCapacity) {
    Frame expect1(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame expect2(0x20, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame expect3(0x30, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});

    Queue<Frame> q(2);
    assertEqual(q.capacity(), (size_t)2);
    assertEqual(q.empty(), true);
    assertEqual(q.peek(), nullptr);

    assertEqual(q.enqueue(expect1), true);
    assertEqual(q.size(),  (size_t)1);
    assertEqual(q.empty(), false);
    assertEqual(q.full(), false);
    assertTrue(*q.peek() == expect1);

    assertEqual(q.enqueue(expect2), true);
    assertEqual(q.size(),  (size_t)2);
    assertEqual(q.empty(), false);
    assertEqual(q.full(), true);
    assertTrue(*q.peek() == expect1);

    assertEqual(q.enqueue(expect3), false);
    assertEqual(q.size(),  (size_t)2);
    assertEqual(q.empty(), false);
    assertEqual(q.full(), true);

    assertTrue(*q.peek() == expect1);
    assertTrue(*q.dequeue() == expect1);
    assertEqual(q.size(), (size_t)1);
    assertEqual(q.empty(), false);
    assertEqual(q.full(), false);

    assertTrue(*q.peek() == expect2);
    assertTrue(*q.dequeue() == expect2);
    assertEqual(q.size(), (size_t)0);
    assertEqual(q.empty(), true);
    assertEqual(q.full(), false);

    assertEqual(q.dequeue(), nullptr);
}

test(QueueTest, MixedUsage) {
    Frame expect1(0x10, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame expect2(0x20, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});
    Frame expect3(0x30, 0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88});

    Queue<Frame> q(2);
    assertTrue(q.enqueue(expect1));
    assertTrue(*q.peek() == expect1);

    assertTrue(q.enqueue(expect2));
    assertTrue(*q.dequeue() == expect1);

    assertTrue(q.enqueue(expect3));
    assertTrue(*q.dequeue() == expect2);
    assertTrue(*q.dequeue() == expect3);
}

test(QueueTest, FrameReserveCapacity) {
    Frame frame(0x10, 0, {});

    Queue<Frame> q(1, 8);
    q.enqueue(frame);
    Frame* actual = q.peek();
    assertEqual(actual->capacity(), 8);
}

test(QueueTest, Alloc) {
    Frame expect(0x10, 0, {0x10, 0x20});

    Queue<Frame> q(1, 8);
    Frame* actual = q.alloc();
    actual->id(0x10);
    actual->data({0x10, 0x20});
    q.enqueue(*actual);

    assertEqual(q.size(), (size_t)1);
    assertTrue(*q.peek() == expect);
    assertTrue(*q.dequeue() == expect);
    assertTrue(q.empty());
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
