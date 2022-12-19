#include "J1939.h"

#include <Arduino.h>
#include <ByteOrder.h>
#include <Canny.h>

namespace Canny {
namespace {

// Return true if a PGN is PDU1.
bool PDU1(uint32_t pgn) {
    return ((pgn >> 8) & 0xFF) < 240;
}

}  // namespace

uint32_t j1939_name_identifier(uint64_t name) {
    return (name & 0xFFFFF80000000000) >> 43;
}

uint16_t j1939_name_manufacturer_code(uint64_t name) {
    return (name & 0x000007FF00000000) >> 32;
}

uint8_t j1939_name_ecu_instance(uint64_t name) {
    return (name & 0x00000000E0000000) >> 29;
}

uint8_t j1939_name_function_instance(uint64_t name) {
    return (name & 0x000000001F000000) >> 24;
}

uint8_t j1939_name_function(uint64_t name) {
    return (name & 0x0000000000FF0000) >> 16;
}

bool j1939_name_reserved(uint64_t name) {
    return (name & 0x0000000000008000) >> 15;
}

uint8_t j1939_name_vehicle_system(uint64_t name) {
    return (name & 0x0000000000007F00) >> 8;
}

uint8_t j1939_name_vehicle_system_instance(uint64_t name) {
    return (name & 0x00000000000000F0) >> 4;
}

uint8_t j1939_name_industry_group(uint64_t name) {
    return (name & 0x000000000000000E) >> 1;
}

bool j1939_name_arbitrary_address(uint64_t name) {
    return name & 0x00000000000000001;
}

J1939Message::J1939Message() : Frame<8, 0xFF>(0x1C0000FF, 1, 0) {}

J1939Message::J1939Message(uint32_t pgn, uint8_t sa, uint8_t da, uint8_t priority) :
    Frame<8, 0xFF>(((priority & 0x07) << 26) | (pgn << 8) | sa | (PDU1(pgn) ? da << 8 : 0x00), 1, 0) {}

void J1939Message::name(uint64_t name) {
    resize(8);
    ByteOrder::hlltonb(data(), name);
}

uint64_t J1939Message::name() const {
    if (size() < 8) {
        return 0;
    }
    return ByteOrder::nbtohll(data());
}

uint8_t J1939Message::priority() const {
    return (id() >> 26) & 0x07; 
}

void J1939Message::priority(uint8_t priority) {
    id((id() & 0xE3FFFFFF) | ((priority & 0x07) << 26));
}

bool J1939Message::reserved() const {
    return (id() & 0x02000000) == 0x02000000;
}

void J1939Message::reserved(bool reserved) {
    if (reserved) {
        id(id() | 0x02000000);
    } else {
        id(id() & 0xFDFFFFFF);
    }
}

bool J1939Message::data_page() const {
    return (id() & 0x01000000) == 0x01000000;
}

void J1939Message::data_page(bool data_page) {
    if (data_page) {
        id(id() | 0x01000000);
    } else {
        id(id() & 0xFEFFFFFF);
    }
}

uint8_t J1939Message::pdu_format() const {
    return id() >> 16 & 0xFF;
}

void J1939Message::pdu_format(uint8_t pf) {
    id((id() & 0xFF00FFFF) | (pf << 16));
}

uint8_t J1939Message::pdu_specific() const {
    return id() >> 8 & 0xFF;
}

void J1939Message::pdu_specific(uint8_t ps) {
    id((id() & 0xFFFF00FF) | (ps << 8));
}

uint8_t J1939Message::group_extension() const {
    if (pdu_format() >= 240) {
        return pdu_specific();
    }
    return 0x00;
}

void J1939Message::group_extension(uint8_t ge) {
    if (pdu_format() >= 240) {
        pdu_specific(ge);
    }
}

uint8_t J1939Message::dest_address() const {
    if (pdu_format() < 240) {
        return pdu_specific();
    }
    return 0xFF;
}

void J1939Message::dest_address(uint8_t da) {
    if (pdu_format() < 240) {
        pdu_specific(da);
    } 
}

uint8_t J1939Message::source_address() const {
    return id() & 0xFF;
}

void J1939Message::source_address(uint8_t sa) {
    id((id() & 0xFFFFFF00) | sa);
}

uint32_t J1939Message::pgn() const {
    uint32_t pgn = pdu_specific() | (pdu_format() << 8) | (data_page() << 16) | (reserved() << 17);
    if (PDU1(pgn)) {
        pgn &= 0xFFFFFF00;
    }
    return pgn;
}

void J1939Message::pgn(uint32_t pgn) {
    if (PDU1(pgn)) {
        id((id() & 0xFC00FFFF) | ((pgn << 8) & 0x03FF0000));
    } else {
        id((id() & 0xFC0000FF) | ((pgn << 8) & 0x03FFFF00));
    }
}

bool J1939Message::broadcast() const {
    return pdu_format() >= 240 || pdu_specific() == BroadcastAddress;
}

bool J1939Message::valid() const {
    return ext() == 1 && size() <= 8;
}

size_t J1939Message::printTo(Print& p) const {
    size_t n = 0;
    n += p.print(id(), HEX);
    n += p.print("#");
    for (int i = 0; i < size(); i++) {
        if (data()[i] <= 0x0F) {
            n += p.print("0");
        }
        n += p.print(data()[i], HEX);
        if (i < size()-1) {
            n += p.print(":");
        }
    }
    return n;
}

}  // namespace Canny
