#pragma once
#include "raylib.h"
#include <variant>

enum class MaskType {
    Circle,
    Rectangle
};

class CollisionMask {
public:
    CollisionMask() = default;

    // Circle constructor
    CollisionMask(Vector2 center, float radius);

    // Rectangle constructor (using Raylib Rectangle: x, y, width, height)
    CollisionMask(Rectangle rect);

    // Update methods (call these every frame when the object moves)
    void SetCircle(Vector2 center, float radius);
    void SetRectangle(Rectangle rect);

    // Getters
    MaskType GetType() const { return type; }
    Vector2 GetCenter() const { return center; }
    float GetRadius() const { return radius; }
    Rectangle GetRect() const { return rect; }

    // The main check
    static bool CheckCollision(const CollisionMask& a, const CollisionMask& b);

private:
    MaskType type = MaskType::Circle;

    // Circle data
    Vector2 center{0.0f, 0.0f};
    float radius = 0.0f;

    // Rectangle data
    Rectangle rect{0.0f, 0.0f, 0.0f, 0.0f};
};