#ifndef _packetEvent_H
#define _packetEvent_H
#include <sst/core/event.h>

/**
 * @brief Enum for the types of messages in the simulation. 
 * 
 */
enum PacketType {
    PACKET,
    LIMIT,
};

/**
 * @brief Packet Structure. Contains a packet type, the id for the packet, and which sender node the packet originated from. 
 * 
 */
struct Packet {
    PacketType type; 
    int id;
    int node_id;
};

/**
 * @brief Custom event type that handles Packet structures.
 * 
 */
class PacketEvent : public SST::Event {

public:

    /**
     * @brief Serialize members of the Packet struct. 
     * 
     * @param ser Wrapper class for objects to declare the order in which their members are serialized/deserialized. 
     */
    void serialize_order(SST::Core::Serialization::serializer &ser) override {
        Event::serialize_order(ser);
        ser & pack.type;
        ser & pack.id;
        ser & pack.node_id;
    }

    PacketEvent(Packet pack) :
        Event(),
        pack(pack)
    {}

    PacketEvent() {} // For serialization.

    Packet pack; // Data type handled by event.

    ImplementSerializable(PacketEvent); // For serialization.
};

#endif