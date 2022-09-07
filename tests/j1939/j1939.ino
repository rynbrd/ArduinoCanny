#include <Arduino.h>
#include <AUnit.h>
#include <Canny/J1939.h>

using namespace aunit;

namespace Canny::J1939 {

test(MessageTest, ConstructBroadcast) {
    // Broadcast PDU.
    Message msg(0xFF12, 0x31);

    // 000 111 0 0 1111111100010010 00110001
    assertEqual(msg.id(), (uint32_t)0x1CFF1231);
    assertEqual(msg.ext(), 1);

    assertEqual(msg.priority(), 0x07);
    assertEqual(msg.reserved(), false);
    assertEqual(msg.data_page(), false);
    assertEqual(msg.pdu_format(), 0xFF);
    assertEqual(msg.pdu_specific(), 0x12);
    assertEqual(msg.group_extension(), 0x12);
    assertEqual(msg.dest_address(), 0xFF);
    assertEqual(msg.source_address(), 0x31);
    assertEqual(msg.pgn(), (uint32_t)0xFF12);
    assertEqual(msg.broadcast(), true);
    assertEqual(msg.valid(), true);
}

test(MessageTest, ConstructAddressable) {
    // P2P PDU with priority of 1.
    Message msg(0xEF00, 0x31, 0x42, 0x01);

    // 000 001 0 0 1110111101000010 00110001
    assertEqual(msg.id(), (uint32_t)0x04EF4231);
    assertEqual(msg.ext(), 1);

    assertEqual(msg.priority(), 0x01);
    assertEqual(msg.reserved(), false);
    assertEqual(msg.data_page(), false);
    assertEqual(msg.pdu_format(), 0xEF);
    assertEqual(msg.pdu_specific(), 0x42);
    assertEqual(msg.group_extension(), 0x00);
    assertEqual(msg.dest_address(), 0x42);
    assertEqual(msg.source_address(), 0x31);
    assertEqual(msg.pgn(), (uint32_t)0xEF00);
    assertEqual(msg.broadcast(), false);
    assertEqual(msg.valid(), true);
}

test(MessageTest, Setters) {
    Message msg(0xFF12, 0x31);
    assertEqual(msg.id(), (uint32_t)0x1CFF1231);

    // 000 001 0 0 11111111 00010010 00110001
    msg.priority(0x01);
    assertEqual(msg.id(), (uint32_t)0x04FF1231);
    assertEqual(msg.priority(), 0x01);

    // 000 001 1 0 11111111 00010010 00110001
    msg.reserved(true);
    assertEqual(msg.id(), (uint32_t)0x06FF1231);
    assertEqual(msg.reserved(), true);

    // 000 001 1 1 11111111 00010010 00110001
    msg.data_page(true);
    assertEqual(msg.id(), (uint32_t)0x07FF1231);
    assertEqual(msg.data_page(), true);

    // 000 001 1 1 11111111 00010011 00110001
    msg.pdu_specific(0x13);
    assertEqual(msg.id(), (uint32_t)0x07FF1331);
    assertEqual(msg.pdu_specific(), 0x13);

    // 000 001 1 1 11111111 00010100 00110001
    msg.group_extension(0x14);
    assertEqual(msg.id(), (uint32_t)0x07FF1431);
    assertEqual(msg.group_extension(), 0x14);
    assertEqual(msg.pdu_specific(), 0x14);

    // noop
    msg.dest_address(0x42);
    assertEqual(msg.id(), (uint32_t)0x07FF1431);
    assertEqual(msg.pdu_specific(), 0x14);

    // 000 001 1 1 11111111 00010100 00110010
    msg.source_address(0x32);
    assertEqual(msg.id(), (uint32_t)0x07FF1432);
    assertEqual(msg.source_address(), 0x32);

    // 000 001 1 1 11101111 00010100 00110010
    msg.pdu_format(0xEF);
    assertEqual(msg.id(), (uint32_t)0x07EF1432);
    assertEqual(msg.pdu_format(), 0xEF);
    assertEqual(msg.pdu_specific(), 0x14);
    assertEqual(msg.group_extension(), 0x00);
    assertEqual(msg.dest_address(), 0x14);

    // 000 001 0 0 11101111 00010100 00110010
    msg.pgn(0xEF22);
    assertEqual(msg.id(), (uint32_t)0x04EF1432);
    assertEqual(msg.reserved(), false);
    assertEqual(msg.data_page(), false);
    assertEqual(msg.pdu_specific(), 0x14);
    assertEqual(msg.group_extension(), 0x00);
    assertEqual(msg.dest_address(), 0x14);

    // 000 001 0 0 11101111 01010101 00110010
    msg.dest_address(0x55);
    assertEqual(msg.id(), (uint32_t)0x04EF5532);
    assertEqual(msg.pdu_specific(), 0x55);
    assertEqual(msg.dest_address(), 0x55);

    Serial.println(msg);
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
