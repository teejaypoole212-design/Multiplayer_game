#include "game.h"

Game game;

uint32_t Game::AddPlayer(const std::string& name) {
    player new_player(name, alive_map);          // auto-generates unique ID
    uint32_t new_id = new_player.id;
    players.emplace(new_player.id, std::move(new_player));
    return new_id;
}

void Game::RemovePlayer(uint32_t id) {
    players.erase(id);
    alive_map.erase(id);
}

void Game::initalize() {
    generate_world(world_object);
}

void Game::Update() {
// First let every player check if they died
    for (auto& [id, p] : players) {
        p.death_check(alive_map);
    }

    // Now safely remove the dead ones
    for (auto it = players.begin(); it != players.end(); ) {
        if (!it->second.IsAlive()) {
            uint32_t dead_id = it->first;
            it = players.erase(it);     // delete from players map
            alive_map.erase(dead_id);   // also delete from alive_map
        } else {
            it->second.update();        // only update living players
            ++it;
        }
    }
    for (auto& [id, p] : players) {
    // id  = uint32_t key
    // p   = player object
        if (check_world_collisions(p.get_mask(), world_object)) {
        // colliding with world -- for now just log it so you can confirm detection works
        p.undo_move(world_object);
        }
    }
    //maybe bullets for other check idk

    auto it1 = players.begin();
    for (; it1 != players.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    for (; it2 != players.end(); ++it2) {
        if (CollisionMask::CheckCollision(it1->second.get_mask(), it2->second.get_mask())) {
            it1->second.collision_check(it2->second.get_mask());
            it2->second.collision_check(it1->second.get_mask());
        }
    }
}
}

void Game::Draw() {
    for (auto& [id, p] : players) {
        if (p.IsAlive()) {
            p.draw(p.GetPosition(), p.GetRotation());
        }
    }
    draw_world(world_object);
}

void Game::KillPlayer(uint32_t id) {
    auto it = players.find(id);
    if (it != players.end()) {
        it->second.Kill();
    }
}


void Game::ApplyRemoteState(uint32_t id, Vector2 position, float rotation) {
    auto it = players.find(id);
    if (it != players.end()) {
        it->second.set_remote_state(position, rotation);
    }
}



void print_alive_map(const std::unordered_map<std::uint32_t, bool>& map) {
    for (const auto& [id, alive] : map) {          // C++17 structured bindings
        std::cout << "ID: " << id << "  alive: " << std::boolalpha << alive << '\n';
    }
}

void Game::AddPlayerWithID(const std::string& name, uint32_t assigned_id) {
    if (players.find(assigned_id) != players.end()) return; // already have them
    player new_player(name, assigned_id);
    players.emplace(assigned_id, std::move(new_player));
    alive_map[assigned_id] = true;
}

void Game::SetPlayerInput(uint32_t id, Vector2 velocity, float rotation) {
    auto it = players.find(id);
    if (it != players.end()) {
        it->second.apply_input(velocity, rotation);
    }
}

void Game::print() {
    //print_alive_map(alive_map);
}