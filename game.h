#pragma once

#include "player.h"
#include "world.h"

#include <unordered_map>
#include <string>
#include <iostream>


void print_alive_map(const std::unordered_map<std::uint32_t, bool>& map);

class Game {
public:
    uint32_t AddPlayer(const std::string& name);

    // NEW: for the client — adds a player using an ID the server already
    // assigned, instead of generating a new one.
    void AddPlayerWithID(const std::string& name, uint32_t assigned_id);

    void RemovePlayer(uint32_t id);
    void Update();
    void Draw();
    void initalize();
    void print();

    uint32_t get_local_player_id() const {return local_player_id;}
    void SetLocalPlayerID(uint32_t id) { local_player_id = id; has_local_player = true; }
    void KillPlayer(uint32_t id);
    void SetPlayerInput(uint32_t id, Vector2 velocity, float rotation);
    void ApplyRemoteState(uint32_t id, Vector2 position, float rotation);
    
    std::unordered_map<uint32_t, player>& GetPlayers() { return players; }
    World& GetWorld() { return world_object; }

private:
    std::unordered_map<uint32_t, player> players;
    std::unordered_map<uint32_t, bool> alive_map;

    World world_object = {};

    Camera2D camera{};
    uint32_t local_player_id = 0;
    bool has_local_player = false;
};

extern Game game;