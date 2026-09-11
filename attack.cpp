#include "attack.h"


attack::attack(int speed, int size, int damage, int pen, Color col,Vector2 pos, Vector2 vel, Vector2 vel_subtract)
: attack_speed(speed), attack_size(size), attack_damage(damage), attack_pen(pen), color(col), pos(pos), vel(vel), vel_subtractor(vel_subtract) {

}


void attack::deal_damage(u_int32_t target_id) {

}


void fire_basic_attack(Vector2 pos, Vector2 vel) {
    attack_list.push_back(attack(1,1,5,1,BLACK, pos, vel, {0,0}));
}

void attack::update() {
    pos += vel;
    if (vel.x > vel_subtractor.x) {vel.x -= vel_subtractor.x;} else {vel.x = 0;} //to update the vel if there is a subtractor 
    if (vel.y > vel_subtractor.y) {vel.y -= vel_subtractor.y;} else {vel.y = 0;} 
}