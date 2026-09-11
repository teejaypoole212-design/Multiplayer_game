#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>
#include "collisions.h"




class attack {
private:

int attack_speed;
int attack_size;
int attack_damage;
int attack_pen;

Vector2 pos = {};
Vector2 vel = {};
Vector2 vel_subtractor = {0,0};
Color color;

bool can_bounce = false; //make a bouncing bullet ability one day
bool is_alive = true;

public:
attack(int speed, int size, int damage, int pen, Color col, Vector2 pos, Vector2 vel, Vector2 vec_subtract);

void make_bouncy() {can_bounce = true;}
void kill_attack() {is_alive = false;}



void deal_damage(u_int32_t target_id);
void update();

protected:
    

};

std::vector<attack> attack_list;

void fire_basic_attack(Vector2 pos, Vector2 vel);