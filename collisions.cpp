#include "collisions.h"

CollisionMask::CollisionMask(Vector2 center, float radius) {
    SetCircle(center, radius);
}

CollisionMask::CollisionMask(Rectangle rect) {
    SetRectangle(rect);
}

void CollisionMask::SetCircle(Vector2 center, float radius) {
    this->type = MaskType::Circle;
    this->center = center;
    this->radius = radius;
}

void CollisionMask::SetRectangle(Rectangle rect) {
    this->type = MaskType::Rectangle;
    this->rect = rect;
}

bool CollisionMask::CheckCollision(const CollisionMask& a, const CollisionMask& b) {
    // Circle vs Circle
    if (a.type == MaskType::Circle && b.type == MaskType::Circle) {
        return CheckCollisionCircles(a.center, a.radius, b.center, b.radius);
    }

    // Rect vs Rect
    if (a.type == MaskType::Rectangle && b.type == MaskType::Rectangle) {
        return CheckCollisionRecs(a.rect, b.rect);
    }

    // Circle vs Rect
    if (a.type == MaskType::Circle && b.type == MaskType::Rectangle) {
        return CheckCollisionCircleRec(a.center, a.radius, b.rect);
    }

    // Rect vs Circle
    if (a.type == MaskType::Rectangle && b.type == MaskType::Circle) {
        return CheckCollisionCircleRec(b.center, b.radius, a.rect);
    }

    return false;
}