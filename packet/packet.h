#ifndef PACKET_H
#define PACKET_H

#include "../enet/include/enet.h"

char* GetTextPointerFromPacket(ENetPacket* packet);
int GetMessageTypeFromPacket(ENetPacket* packet);
void SendPacketPacket(ENetPacket* oldPacket, ENetPeer* peer);
void sendPacket(int val, char* packetText, ENetPeer* peer);
unsigned char* GetExtendedDataPointerFromTankPacket(unsigned char* a1);
void enet_peerSend(ENetPacket* packet, ENetPeer* peer);
ENetPacket* onPacketCreate(char* format, ...);

struct GameUpdatePacket { // https://github.com/SrMotion/INZERNAL/blob/main/INZERNAL/sdk/GameUpdatePacket.h
    uint8_t type; //0
    union {
        uint8_t objtype; //1
        uint8_t punchid;
        uint8_t npctype;
    };
    union {
        uint8_t count1; //2
        uint8_t jump_count;
        uint8_t build_range;
        uint8_t npc_id;
        uint8_t lost_item_count;
    };
    union {
        uint8_t count2; //3
        uint8_t animation_type;
        uint8_t punch_range;
        uint8_t npc_action;
        uint8_t particle_id;
        uint8_t gained_item_count;
        uint8_t dice_result;
        uint8_t fruit_count;
    };
    union {
        int32_t netid; //4
        int32_t effect_flags_check;
        int32_t object_change_type;
        int32_t particle_emitter_id;
    };
    union {
        int32_t item; //8
        int32_t ping_hash;
        int32_t item_netid;
        int32_t pupil_color;
        int32_t tiles_length;
    };
    int32_t flags; //12 - 0xC
    union {
        int int_var;
        float float_var; //16 - 0x10
        float water_speed;
        float obj_alt_count;
    };
    union {
        int32_t int_data; //20 - 0x14
        int32_t ping_item;
        int32_t elapsed_ms;
        int32_t delay;
        int32_t tile_damage;
        int32_t item_id;
        int32_t item_speed;
        int32_t effect_flags;
        int32_t object_id;
        int32_t hash;
        int32_t verify_pos;
        int32_t client_hack_type;
    };
    union {
        float vec_x; //24 - 0x18
        float pos_x;
        float accel;
        float punch_range_in;
    };
    union {
        float vec_y; //28 - 0x1C
        float pos_y;
        float build_range_in;
        float punch_strength;
    };
    union {
        float vec2_x; //32 - 0x20
        float dest_x;
        float gravity_in;
        float speed_out;
        float velocity_x;
        float particle_variable;
        float pos2_x;
        int hack_type;
    };
    union {
        float vec2_y; //36 - 0x24
        float dest_y;
        float speed_in;
        float gravity_out;
        float velocity_y;
        float particle_alt_id;
        float pos2_y;
        int hack_type2;
    };
    union {
        float particle_rotation; //40 - 0x28
        float npc_variable;
    };
    union {
        uint32_t int_x; //44 - 0x2C
        uint32_t item_id_alt;
        uint32_t eye_shade_color;
    };
    union {
        uint32_t int_y; //48 - 0x30
        uint32_t item_count;
        uint32_t eyecolor;
        uint32_t npc_speed;
        uint32_t particle_size_alt;
    };
    uint32_t data_size; //52
    uint32_t data; // 56
};

#endif // PACKET_H
