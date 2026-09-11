#include "raylib.h"
#include <enet/enet.h>
#include <iostream>
#include <cstring>

#include "game.h"
#include "protocol.h"
#include "world.h"

int main() {
SetRandomSeed(time(NULL));
    if (enet_initialize() != 0) {
        std::cout << "an error ocured initializing enet" << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress adress;
    ENetEvent event;

    adress.host = ENET_HOST_ANY;
    adress.port = 7777;
    ENetHost* server;
    server = enet_host_create(&adress, 128, 1, 0, 0);

    if (server == NULL) {
        std::cout << "server could not be created" << std::endl;
        return EXIT_FAILURE;
    }

    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Multiplayer Game Host");
    SetTargetFPS(60);

    game.initalize(); // generates the world once, server-side

    while (!WindowShouldClose()) {

        while (enet_host_service(server, &event, 0) > 0) {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT: {
                std::cout << "a new client connected from " << event.peer->address.host << ":" << event.peer->address.port << std::endl;

                uint32_t new_id = game.AddPlayer("player");
                std::cout << "player assigned id: " << new_id << std::endl;

                // Remember which player this connection owns, so future
                // packets from this peer know who they belong to.
                event.peer->data = reinterpret_cast<void*>(static_cast<uintptr_t>(new_id));

                // Tell the client their assigned ID.
                WelcomePacket welcome;
                welcome.your_id = new_id;
                ENetPacket* welcome_pkt = enet_packet_create(&welcome, sizeof(welcome), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, welcome_pkt);

                // Send the client the actual world grid so both sides match.
                ENetPacket* world_pkt = enet_packet_create(&game.GetWorld(), sizeof(World), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, world_pkt);

                break;
            }

            case ENET_EVENT_TYPE_RECEIVE: {
                if (event.packet->dataLength == sizeof(input_packet)) {
                    input_packet in;
                    std::memcpy(&in, event.packet->data, sizeof(input_packet));

                    uint32_t id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(event.peer->data));

                    // Apply the input to this player. If Game doesn't have
                    // SetPlayerInput yet, add a method that sets that
                    // player's vel/rotation directly.
                    game.SetPlayerInput(id, { in.vel_x, in.vel_y }, in.rotation);
                }

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT: {
                uint32_t id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(event.peer->data));
                std::cout << "player " << id << " disconnected" << std::endl;
                game.KillPlayer(id);
                break;
            }
            default:
                break;
            }
        }

        game.Update();
    {
    auto& players = game.GetPlayers();

    std::vector<uint8_t> buffer(sizeof(SnapshotHeader) + players.size() * sizeof(PlayerState));

    SnapshotHeader header;
    header.player_count = static_cast<uint32_t>(players.size());
    std::memcpy(buffer.data(), &header, sizeof(header));

    size_t offset = sizeof(header);
    for (auto& [id, p] : players) {
        PlayerState ps;
        ps.id = id;
        ps.x = p.GetPosition().x;
        ps.y = p.GetPosition().y;
        ps.rotation = p.GetRotation();
        std::memcpy(buffer.data() + offset, &ps, sizeof(ps));
        offset += sizeof(ps);
    }

    ENetPacket* snap_pkt = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_UNSEQUENCED);
    enet_host_broadcast(server, 0, snap_pkt);

}
        BeginDrawing();
        ClearBackground(RAYWHITE);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    enet_host_destroy(server);

    return 0;
}