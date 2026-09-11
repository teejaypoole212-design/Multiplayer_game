#pragma once

#pragma pack(push,1)
struct  input_packet
{
    float vel_x;
    float vel_y;
    float rotation;
};

struct WelcomePacket {
    uint32_t your_id;
};

struct PlayerState {
    uint32_t id;
    float x;
    float y;
    float rotation;
};

struct SnapshotHeader {
    uint32_t player_count;
};
#pragma pack(pop)