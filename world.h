#pragma once

#include <array>
#include "raylib.h"

#include "collisions.h"
#include <iostream>

constexpr int BLOCK_SIZE = 64;   // was hardcoded 10 in draw_world
constexpr int WORLD_CELLS = 250;

using World = std::array<std::array<int, WORLD_CELLS>, WORLD_CELLS>;

void generate_world(World& world);

void draw_world(World& world);

bool check_world_collisions(const CollisionMask& other, const World& world);


Vector2 find_free_position(Vector2 start, float radius, const World& world);