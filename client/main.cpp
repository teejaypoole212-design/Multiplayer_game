#include <iostream>
#include <cstring>
#include <vector>

#include "enet/enet.h"
#include "raylib.h"
#include "raymath.h"
#include "protocol.h"
#include "game.h"
#include "world.h"

int main() {

    if (enet_initialize() != 0) {
        std::cout << "an error ocured initializing enet" << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost* client;
    client = enet_host_create(NULL, 1, 1, 0, 0);

    if (client == NULL) {
        std::cout << "client could not create host" << std::endl;
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 7777;
    peer = enet_host_connect(client, &address, 1, 0);

    if (peer == NULL) {
        std::cout << "no avalible peers for creating enet connection" << std::endl;
        return EXIT_FAILURE;
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "connection sucsessfull!" << std::endl;
    } else {
        enet_peer_reset(peer);
        std::cout << "connection failed" << std::endl;
        return EXIT_SUCCESS;
    }

    // Block here until we know our own ID AND have the real world data.
    // Nothing meaningful to draw or simulate before both arrive.
    uint32_t my_id = 0;
    bool got_welcome = false;
    bool got_world = false;

    while (!(got_welcome && got_world)) {
        if (enet_host_service(client, &event, 3000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE) {
            if (event.packet->dataLength == sizeof(WelcomePacket)) {
                WelcomePacket w;
                std::memcpy(&w, event.packet->data, sizeof(w));
                my_id = w.your_id;
                got_welcome = true;
                std::cout << "assigned id: " << my_id << std::endl;
            }
            else if (event.packet->dataLength == sizeof(World)) {
                std::memcpy(&game.GetWorld(), event.packet->data, sizeof(World));
                got_world = true;
                std::cout << "world received" << std::endl;
            }
            enet_packet_destroy(event.packet);
        }
    }

    game.AddPlayerWithID("You", my_id);
    game.SetLocalPlayerID(my_id);

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Multiplayer Client");
    SetTargetFPS(60);


    Camera2D camera{};
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    while (!WindowShouldClose()) {

        while (enet_host_service(client, &event, 0) > 0) {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE: {
                if (event.packet->dataLength >= sizeof(SnapshotHeader)) {
                    SnapshotHeader header;
                    std::memcpy(&header, event.packet->data, sizeof(header));

                    size_t offset = sizeof(header);
                    for (uint32_t i = 0; i < header.player_count; i++) {
                        if (offset + sizeof(PlayerState) > event.packet->dataLength) break;

                        PlayerState ps;
                        std::memcpy(&ps, event.packet->data + offset, sizeof(ps));
                        offset += sizeof(ps);

                        if (ps.id == my_id) continue; // never let the network override your own local player

                        if (game.GetPlayers().find(ps.id) == game.GetPlayers().end()) {
                            game.AddPlayerWithID("Player", ps.id);
                        }
                        game.ApplyRemoteState(ps.id, { ps.x, ps.y }, ps.rotation);
                    }
                }
                enet_packet_destroy(event.packet);
                break;
            }
            default:
                break;
            }
        }


        input_packet in;
        Vector2 move_input = {
        static_cast<float>(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)),
        static_cast<float>(IsKeyDown(KEY_S) - IsKeyDown(KEY_W))
        };

        if (move_input.x != 0.0f || move_input.y != 0.0f) {
            move_input = Vector2Normalize(move_input);
        }

        in.vel_x = move_input.x * 200.0f;
        in.vel_y = move_input.y * 200.0f;

        Vector2 mouse_screen = GetMousePosition();
        Vector2 mouse_world = GetScreenToWorld2D(mouse_screen, camera);

        Vector2 my_pos = game.GetPlayers().at(my_id).GetPosition();
        in.rotation = atan2f(mouse_world.y - my_pos.y, mouse_world.x - my_pos.x) * RAD2DEG;
        

        ENetPacket* packet = enet_packet_create(&in, sizeof(in), ENET_PACKET_FLAG_UNSEQUENCED);
        enet_peer_send(peer, 0, packet);

        game.GetPlayers().at(my_id).update();
        game.GetPlayers().at(my_id).SetRotation(in.rotation);

        if (check_world_collisions(game.GetPlayers().at(my_id).get_mask(), game.GetWorld())) {
        game.GetPlayers().at(my_id).undo_move(game.GetWorld());
        }
        camera.target = game.GetPlayers().at(my_id).GetPosition();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        game.Draw();
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    enet_peer_disconnect(peer, 0);
    while (enet_host_service(client, &event, 3000) > 0) {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "disconect form server succsess!" << std::endl;
            
            break;
        default:
            break;
        }
    }

    return 0;
}