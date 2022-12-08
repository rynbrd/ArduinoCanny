#ifndef _CANNY_J1939_H_
#define _CANNY_J1939_H_

#include <Arduino.h>
#include <Canny.h>

namespace Canny {

// The J1939 null address. Used during address claim by devices that have not
// yet claimed an address.
const uint8_t NullAddress = 254;

// The J1939 broadcast address. Used as a global broadcast address for some
// PDU1 messages.
const uint8_t BroadcastAddress = 255;

// Return the Identifier part of a J1939 NAME.
uint32_t j1939_name_identifier(uint64_t name);

// Return the Manufacturer Code part of a J1939 NAME.
uint16_t j1939_name_manufacturer_code(uint64_t name);

// Return the ECU Instance part of a J1939 NAME.
uint8_t j1939_name_ecu_instance(uint64_t name);

// Return the Function Instance part of a J1939 NAME.
uint8_t j1939_name_function_instance(uint64_t name);

// Return the Function part of a J1939 NAME.
uint8_t j1939_name_function(uint64_t name);

// Return the Reserved bit of a J1939 NAME.
bool j1939_name_reserved(uint64_t name);

// Return the Vehicle System part of a J1939 NAME.
uint8_t j1939_name_vehicle_system(uint64_t name);

// Return the Vehicle System Instance part of a J1939 NAME.
uint8_t j1939_name_vehicle_system_instance(uint64_t name);

// Return the Industry Group part of a J1939 NAME.
uint8_t j1939_name_industry_group(uint64_t name);

// Return the Arbitrary Address bit of a J1939 NAME.
bool j1939_name_arbitrary_address(uint64_t name);

// A J1939 message. All J1939 messages are CAN frames with an extended ID space
// and an 8-byte payload.
class J1939Message : public Frame<8> {
    public:
        // Construct an empty message with no capacity. Its data is set to
        // nullptr and its capacity to 0. Priority defaults to 111b, PGN to
        // 0, and SA to the null address.
        J1939Message();

        // Construct an empty message with the given PGN, Source Address,
        // Destination Address, and Priority. The DA value is ignored for PDU2
        // PGNs. Priority defaults to 111b.
        J1939Message(uint32_t pgn, uint8_t sa, uint8_t da = 0x00, uint8_t priority = 0x07);

        // Set the message data to a J1939 name.
        void name(uint64_t name);

        // Return the message data as a J1939 name.
        uint64_t name() const;

        // Return the Priority value.
        uint8_t priority() const;

        // Set the Priority. The Priority is a 3-bit value so
        // only the lowest three bits are used.
        void priority(uint8_t priority);

        // Return the value of the Reserved bit.
        bool reserved() const;

        // Set the reserved bit.
        void reserved(bool reserved);

        // Return the value of the Data Page bit.
        bool data_page() const;

        // Set the Data Page bit.
        void data_page(bool data_page);

        // Return the PDU Format (PF) field value. PDU1 messages have a PF
        // between 0 and 239. PDU2 messages have a PF between 240 and 255.
        uint8_t pdu_format() const;

        // Set the PDU Format (PF) field.
        void pdu_format(uint8_t pf);

        // Return the PDU Specific (PS) field value. This contains the
        // Destination Address (DA) for addressable (PDU1) messages or the
        // Group Extension for broadcast (PDU2) messages.
        uint8_t pdu_specific() const;

        // Set the PDU Specific (PS) field.
        void pdu_specific(uint8_t ps);

        // Return the Group Extension for a PDU2 message. Return 0x00 for a
        // PDU1 message.
        uint8_t group_extension() const;

        // Set the Group Extension for a PDU2 message. This is a noop for a
        // PDU1 message.
        void group_extension(uint8_t ge);

        // Return the destination address for PDU1 a message. Return 0xFF for a
        // PDU2 message.
        uint8_t dest_address() const;

        // Set the Destination Address for a PDU1 message. This is a noop for
        // PDU2 messages.
        void dest_address(uint8_t da);

        // Return the Source Address (SA) field value.
        uint8_t source_address() const;

        // Set the Source Address (SA) field.
        void source_address(uint8_t sa);

        // Return the 18-bit message PGN.
        //
        // The PS byte is always 0x00 for PDU1 messages.
        uint32_t pgn() const;

        // Set the message PGN. This sets the Reserved Bit, Data Page, and PDU
        // Format fields. The PDU Specific field is set for PDU2 PGNs.
        //
        // The PS byte is ignored for PDU1 PGNs and should be 0x00 for consistency.
        void pgn(uint32_t pgn);

        // Return true if this is a broadcast message. Broadcast messages are
        // PDU1 messages with a broadcast DA or PDU2 messages.
        bool broadcast() const;

        // Return false if this is not a valid J1939 message. This checks the
        // ext flag and data size.
        bool valid() const;

        // Write a human readable string representation of the J1939 message to
        // a print object. Return the number of bytes written. Implements
        // Printable.
        size_t printTo(Print& p) const override;
};

}  // namespace Canny

#endif  // _CANNY_J1939_H_
