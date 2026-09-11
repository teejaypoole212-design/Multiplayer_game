#include "world.h"

const int world_size = WORLD_CELLS;



void generate_world(World& world) {
    // First pass: randomly place blocks
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            world[x][y] = (GetRandomValue(0, 15) == 1) ? 1 : 0;
        }
    }
    // Second pass: remove blocks only if they have too many neighbors
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (world[x][y] == 0) continue;

            int neighbors = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;

                    // bounds check
                    if (nx < 0 || nx >= world_size || ny < 0 || ny >= world_size) continue;

                    if (world[nx][ny] == 1) {
                        neighbors++;
                    }
                }
            }

            // tweak threshold: higher = more open space, lower = denser clusters
            if (neighbors >= 5) {
                world[x][y] = 0;
            }
        }
    }
}

// 1 = 10 pixles

void draw_world(World& world) {

    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (world[x][y] == 1) {
                float cx = x * BLOCK_SIZE + BLOCK_SIZE / 2.0f;
                float cy = y * BLOCK_SIZE + BLOCK_SIZE / 2.0f;

                DrawRectangle(static_cast<int>(cx - BLOCK_SIZE / 2.0f), static_cast<int>(cy - BLOCK_SIZE / 2.0f), BLOCK_SIZE, BLOCK_SIZE, DARKGRAY);
            }
        }
    }
}


// Returns true if the given mask collides with any solid block in the world
bool check_world_collisions(const CollisionMask& other, const World& world) {
    Vector2 center;
    float radius = 0.0f;

    if (other.GetType() == MaskType::Circle) {
        center = other.GetCenter();
        radius = other.GetRadius();
    }
    else if (other.GetType() == MaskType::Rectangle) {
        Rectangle r = other.GetRect();
        center = { r.x + r.width * 0.5f, r.y + r.height * 0.5f };
        radius = std::max(r.width, r.height) * 0.5f;
    }
    else {
        return false;
    }

    // Convert pixel-space center into grid-space indices before scanning.
    int gx = static_cast<int>(center.x / BLOCK_SIZE);
    int gy = static_cast<int>(center.y / BLOCK_SIZE);
    int range = static_cast<int>(radius / BLOCK_SIZE) + 2;

    int min_x = std::max(0, gx - range);
    int max_x = std::min(WORLD_CELLS - 1, gx + range);
    int min_y = std::max(0, gy - range);
    int max_y = std::min(WORLD_CELLS - 1, gy + range);

    for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
            if (world[x][y] == 0) continue;

            CollisionMask block;
            float bx = x * BLOCK_SIZE;
            float by = y * BLOCK_SIZE;
            block.SetRectangle({ bx, by, static_cast<float>(BLOCK_SIZE), static_cast<float>(BLOCK_SIZE) });

            if (CollisionMask::CheckCollision(other, block)) {
                return true;
            }
        }
    }

    return false;
}



Vector2 find_free_position(Vector2 start, float radius, const World& world) {
    CollisionMask test;
    test.SetCircle(start, radius);
    if (!check_world_collisions(test, world)) return start;

    int start_gx = static_cast<int>(start.x / BLOCK_SIZE);
    int start_gy = static_cast<int>(start.y / BLOCK_SIZE);

    for (int ring = 1; ring < WORLD_CELLS; ring++) {
        for (int dx = -ring; dx <= ring; dx++) {
            for (int dy = -ring; dy <= ring; dy++) {
                if (std::abs(dx) != ring && std::abs(dy) != ring) continue;

                int gx = start_gx + dx;
                int gy = start_gy + dy;
                if (gx < 0 || gx >= WORLD_CELLS || gy < 0 || gy >= WORLD_CELLS) continue;

                Vector2 candidate = { gx * BLOCK_SIZE + BLOCK_SIZE / 2.0f, gy * BLOCK_SIZE + BLOCK_SIZE / 2.0f };
                test.SetCircle(candidate, radius);
                if (!check_world_collisions(test, world)) return candidate;
            }
        }
    }
    return start;
}