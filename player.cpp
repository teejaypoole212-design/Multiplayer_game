#include "player.h"
#include <string>

#include "game.h"

//checkers ======================================================================================
void player::level_up_check() {
    if (xp_amount >= xp_cap) {
        level += 1;
        xp_amount -= player::xp_cap;
        xp_cap *= 1.1;
    }
}

void player::death_check(std::unordered_map<std::uint32_t, bool>& alive_map) {
    if (hp <= 0 && is_alive) {
        is_alive = false;
        alive_map[id] = false;
    }
}


//===============================================================================================


//updates========================================================================================


void player::update_mask() {
    mask.SetCircle(player::GetPosition(), 10.0f); //asumes the player is a circle with 10 raduis (hardcoded :( )
}

void player::update_move() {
    //gets and watches for input from player, then updates vel
    if (id == game.get_local_player_id()) {
    vel = {0.0f, 0.0f};  // reset every frame

    float speed = static_cast<float>(spd);  // use your spd stat

    if (IsKeyDown(KEY_W)) vel.y -= 1.0f;
    if (IsKeyDown(KEY_S)) vel.y += 1.0f;
    if (IsKeyDown(KEY_A)) vel.x -= 1.0f;
    if (IsKeyDown(KEY_D)) vel.x += 1.0f;

    // Normalize so diagonal movement isn't faster
    if (vel.x != 0.0f || vel.y != 0.0f) {
        vel = Vector2Normalize(vel);
        vel.x *= speed;
        vel.y *= speed;
    }
    } else {
        //if the player isnt local (i made it happen local on client)
    }
}

void player::update_gun_position() {
    // Convert rotation to radians
    float rad = rotation * DEG2RAD;

    // Gun tip position = player center + direction * length
    gun_pos.x = pos.x + cosf(rad) * gun_length;
    gun_pos.y = pos.y + sinf(rad) * gun_length;
}

void player::update_rotation() {
    if (id == game.get_local_player_id()) {
    Vector2 mouse = GetMousePosition();
    float target_angle = atan2f(mouse.y - pos.y, mouse.x - pos.x) * RAD2DEG;

    float diff = target_angle - rotation;

    while (diff > 180.0f)  diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;

    rotation += diff * 0.45f;

    if (rotation > 180.0f)  rotation -= 360.0f;
    if (rotation < -180.0f) rotation += 360.0f;
    } else {
        //if the player is non local 
    }
}

//===============================================================================================

//misc===========================================================================================

void player::GenerateID(std::unordered_map<std::uint32_t, bool>& alive_map) {
    static std::uint32_t next_id = 1;

    // Keep increasing until we find an ID that doesn't exist
    while (alive_map.find(next_id) != alive_map.end()) {
        next_id++;
    }

    id = next_id;
    alive_map[id] = true;   // register as alive
    next_id++;
}

void player::apply_input(Vector2 velocity, float target_rotation) {
    vel = velocity;
    rotation = target_rotation;
}

void player::set_remote_state(Vector2 position, float new_rotation) {
    pos = position;
    rotation = new_rotation;
    update_mask();
    update_gun_position();
}


void player::undo_move(const World& world) {
    Vector2 try_x_only = { pos.x, pos_before_move.y };
    mask.SetCircle(try_x_only, 10.0f);
    if (!check_world_collisions(mask, world)) {
        pos = try_x_only;
        update_mask();
        update_gun_position();
        return;
    }

    Vector2 try_y_only = { pos_before_move.x, pos.y };
    mask.SetCircle(try_y_only, 10.0f);
    if (!check_world_collisions(mask, world)) {
        pos = try_y_only;
        update_mask();
        update_gun_position();
        return;
    }

    // Both single-axis options are blocked. Normally this means "revert
    // fully, you hit a wall dead-on." But if pos_before_move is ALSO
    // colliding (e.g. you spawned inside a block), reverting there would
    // just leave you stuck. So instead, search outward for the nearest
    // actually-open spot and teleport there.
    mask.SetCircle(pos_before_move, 10.0f);
    if (check_world_collisions(mask, world)) {
        pos = find_free_position(pos_before_move, 10.0f, world);
    } else {
        pos = pos_before_move;
    }
    update_mask();
    update_gun_position();
}


void player::collision_check(const CollisionMask& other) {
    // Simplest option: just revert this frame's movement, same as
    // hitting a wall -- stops players from overlapping/walking through
    // each other. No push-apart physics, just a hard stop.
    pos = pos_before_move;
    update_mask();
    update_gun_position();
}
//===============================================================================================


player::player(std::string n, std::unordered_map<std::uint32_t, bool>& alive_map) : name(n) {
    GenerateID(alive_map);

    // Random color
    color = {
        static_cast<unsigned char>(GetRandomValue(50, 255)),  // R
        static_cast<unsigned char>(GetRandomValue(50, 255)),  // G
        static_cast<unsigned char>(GetRandomValue(50, 255)),  // B
        255                                                   // A (fully opaque)
    };
}

player::player(std::string n, uint32_t assigned_id) : name(n) {
    id = assigned_id;
    color = {
    static_cast<unsigned char>(GetRandomValue(50, 255)),  // R
    static_cast<unsigned char>(GetRandomValue(50, 255)),  // G
    static_cast<unsigned char>(GetRandomValue(50, 255)),  // B
    255                                                   // A (fully opaque)
    };
}

void player::move() {
    //moves the player off of the current player vel 
    float dt = GetFrameTime();  // framerate independent
    pos_before_move = pos; 
    pos.x += vel.x * dt;
    pos.y += vel.y * dt;
}

void player::draw(Vector2 pos, float rotation) {
    if (hp < 0) return;
// Draw the body (circle)
    DrawCircleV(pos, 10.0f, color);

    // Draw a small outline so it’s easier to see
    DrawCircleLines(pos.x, pos.y, 10.0f, BLACK);

    // Draw the gun (line from center to gun tip)
    DrawLineEx(pos, gun_pos, 4.0f, DARKGRAY);

    // Optional: draw a small circle at the gun tip
    DrawCircleV(gun_pos, 3.0f, BLACK);

    // Draw the player name above them
    DrawText(name.c_str(), pos.x - 20, pos.y - 25, 10, BLACK);
}

void player::update() { //just put all of needed update funcs inside for universal update
if (hp < 0) return; 
update_move();
update_rotation();   
move();
update_mask();
level_up_check();
update_gun_position();

}



