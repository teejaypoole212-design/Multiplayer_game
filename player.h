#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include "collisions.h"
#include <unordered_map>
#include <cstdint>
#include "raymath.h"
#include "world.h" 

class Game;

//foward decleaation ^^^

//player class- 

//contains: player level, xp, pos, vel, stats(def,atk,spd,etc), direction, name

//note for later i want the player a circle with a gun and their name in the middle :) 

class player {
private:

public:

CollisionMask mask;

player(std::string n, std::unordered_map<uint32_t, bool>& alive_map);
player(std::string n, uint32_t assigned_id);

// Level & XP
int GetLevel() const { return level; }
int GetXP() const { return xp_amount; }
int GetXPCap() const { return xp_cap; }

// Position & Movement
Vector2 GetPosition() const { return pos; }
Vector2 GetVelocity() const { return vel; }
float GetRotation() const { return rotation; }
CollisionMask get_mask() const {return mask;}

// Stats
double GetAttack() const { return atk; }
double GetDefense() const { return def; }
double GetSpeed() const { return spd; }
double GetHP() const { return hp; }
bool IsAlive() const { return is_alive; }

// Name
const std::string& GetName() const { return name; }
std::uint32_t id = 0;
bool is_alive = true;
Color get_color() {return color;}

// Call this in the constructor
void GenerateID(std::unordered_map<std::uint32_t, bool>& alive_map);
void apply_input(Vector2 velocity, float target_rotation);
void set_remote_state(Vector2 position, float new_rotation);

void draw(Vector2 pos, float rotation);
void update();//unkown paramaters for now
void SetRotation(float r) {
    rotation = r;
    update_gun_position();
}

Vector2 pos_before_move{}; // stores position at the start of each move, for collision revert


//DEBUG
void force_move() {
    pos.x += 200;
}



void undo_move(const World& world);
void Kill() { hp = 0; }

void update_mask();
void update_move();
void update_rotation();
void update_gun_position();

void level_up_check();
void death_check(std::unordered_map<std::uint32_t, bool>& alive_map);
void collision_check(const CollisionMask& other);

protected:

    Color color;

    Vector2 gun_pos{};        // where attacks will come from
    float gun_length = 25.0f; // how far the gun sticks out

    int level = 1;
    int xp_amount = 0;
    int xp_cap = 100;

    Vector2 pos = {};//{GetRandomValue(100,980),GetRandomValue(100,980)};
    Vector2 vel{}; //x,y 1^ and -1 is down - etc
    float rotation = 0.0f;

    double atk = 1, def = 1, spd = 200, hp = 100;

    std::string name;


    void move();
    
     
};
