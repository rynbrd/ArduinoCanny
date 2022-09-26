#include <Arduino.h>
#include <AUnit.h>
#include <Canny/J1939.h>

using namespace aunit;

namespace Canny {

test(J1939NameTest, Identifier) {
    uint64_t name = 0x06876A340082FAC0;
    uint32_t expect = 53485;
    assertEqual(j1939_name_identifier(name), expect);
}

test(J1939NameTest, ManufacturerCode) {
    uint64_t name = 0x06876A340082FAC0;
    uint16_t expect = 564;
    assertEqual(j1939_name_manufacturer_code(name), expect);
}

test(J1939NameTest, ECUInstance) {
    uint64_t name = 0x06876A340082FAC0;
    uint8_t expect = 0;
    assertEqual(j1939_name_ecu_instance(name), expect);
}

test(J1939NameTest, FunctionInstance) {
    uint64_t name = 0x06876A340082FAC0;
    uint8_t expect = 0;
    assertEqual(j1939_name_function_instance(name), expect);
}

test(J1939NameTest, Function) {
    uint64_t name = 0x06876A340082FAC0;
    uint8_t expect = 130;
    assertEqual(j1939_name_function(name), expect);
}

test(J1939NameTest, Reserved) {
    uint64_t name = 0x06876A340082FAC0;
    bool expect = true;
    assertEqual(j1939_name_reserved(name), expect);
}

test(J1939NameTest, VehicleSystem) {
    uint64_t name = 0x06876A340082FAC0;
    uint8_t expect = 122;
    assertEqual(j1939_name_vehicle_system(name), expect);
}

test(J1939NameTest, VehicleSystemInstance) {
    uint64_t name = 0x06876A340082FAC0;
    uint8_t expect = 12;
    assertEqual(j1939_name_vehicle_system_instance(name), expect);
}

test(J1939NameTest, IndustryGroup) {
    uint64_t name = 0x06876A340082FAC0;
    uint8_t expect = 0;
    assertEqual(j1939_name_industry_group(name), expect);
}

test(J1939NameTest, ArbitraryAddress) {
    uint64_t name = 0x06876A340082FAC1;
    bool expect = true;
    assertEqual(j1939_name_arbitrary_address(name), expect);
}

test(J1939MessageTest, ConstructBroadcast) {
    // Broadcast PDU.
    J1939Message msg(0xFF12, 0x31);

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

test(J1939MessageTest, ConstructAddressable) {
    // P2P PDU with priority of 1.
    J1939Message msg(0xEF00, 0x31, 0x42, 0x01);

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

test(J1939MessageTest, Setters) {
    J1939Message msg(0xFF12, 0x31);
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
}

test(J1939MessageTest, PGN) {
    J1939Message msg;
    msg.id(0x19F0140A);
    msg.data({0x20, 0x86, 0x35, 0x08, 0x8E, 0x12, 0x4D, 0x53});
    assertEqual(msg.pgn(), (uint32_t)0x1F014);
    assertEqual(msg.pdu_format(), 0xF0);
    assertEqual(msg.pdu_specific(), 0x14);
    assertEqual(msg.dest_address(), 0xFF);
    assertEqual(msg.source_address(), 0x0A);
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
