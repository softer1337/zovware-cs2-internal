#pragma once
#include "../imgui/imgui.h"
#include <cmath>
#include <limits>

#define DEG2RAD(x) ((x) * 3.14159265359f / 180.f)

struct Vec2
{
    float x, y;

    constexpr Vec2(float x = 0.f, float y = 0.f) noexcept
        : x(x), y(y) {
    }

    float Length() const
    {
        return std::sqrt(x * x + y * y);
    }

    float LengthSqr() const
    {
        return x * x + y * y;
    }

    bool IsZero(float eps = 0.0001f) const
    {
        return std::abs(x) < eps && std::abs(y) < eps;
    }

    Vec2 operator+(const Vec2& v) const { return { x + v.x, y + v.y }; }
    Vec2 operator-(const Vec2& v) const { return { x - v.x, y - v.y }; }
    Vec2 operator*(float f) const { return { x * f, y * f }; }
    Vec2 operator/(float f) const { return { x / f, y / f }; }
};

struct Vec3
{
    float x, y, z;

    constexpr Vec3(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
        : x(x), y(y), z(z) {
    }

    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    float LengthSqr() const
    {
        return x * x + y * y + z * z;
    }

    bool IsZero(float eps = 0.0001f) const
    {
        return std::abs(x) < eps &&
            std::abs(y) < eps &&
            std::abs(z) < eps;
    }

    [[nodiscard]] Vec3 Normalize() const
    {
        const float flLength = this->Length();
        return *this / flLength;
    }

    float NormalizeInPlace()
    {
        const float flLength = this->Length();
        const float flRadius = 1.0f / (flLength + std::numeric_limits<float>::epsilon());

        this->x *= flRadius;
        this->y *= flRadius;
        this->z *= flRadius;

        return flLength;
    }

    Vec3 operator+(const Vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
    Vec3 operator-(const Vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
    Vec3 operator*(float f) const { return { x * f, y * f, z * f }; }
    Vec3 operator/(float f) const { return { x / f, y / f, z / f }; }
};

inline bool WorldToScreen(const Vec3& pos, Vec2& screen, const float viewMatrix[4][4]) {
    const float x = pos.x;
    const float y = pos.y;
    const float z = pos.z;

    float clipX = viewMatrix[0][0] * x + viewMatrix[0][1] * y + viewMatrix[0][2] * z + viewMatrix[0][3];
    float clipY = viewMatrix[1][0] * x + viewMatrix[1][1] * y + viewMatrix[1][2] * z + viewMatrix[1][3];
    float clipW = viewMatrix[3][0] * x + viewMatrix[3][1] * y + viewMatrix[3][2] * z + viewMatrix[3][3];

    if (clipW < 0.001f)
        return false;

    float invW = 1.0f / clipW;
    float ndcX = clipX * invW;
    float ndcY = clipY * invW;

    const ImVec2 display = ImGui::GetIO().DisplaySize;
    screen.x = (display.x * 0.5f) * (1.0f + ndcX);
    screen.y = (display.y * 0.5f) * (1.0f - ndcY); // y инвертирован

    return true;
}


inline void NormalizeAngles(Vec3& ang) {
    if (ang.x > 89.0f) ang.x = 89.0f;
    if (ang.x < -89.0f) ang.x = -89.0f;
    while (ang.y > 180.0f) ang.y -= 360.0f;
    while (ang.y < -180.0f) ang.y += 360.0f;
    ang.z = 0.0f;
}