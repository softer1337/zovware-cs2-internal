#pragma once
#include <corecrt_math_defines.h>
#include <cmath>
#include "math.hpp"

class QAngle {
public:
    float x, y, z;

    QAngle() {
        x = y = z = 0.0f;
    }

    QAngle(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }

    QAngle(const QAngle& v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    QAngle& operator=(const QAngle& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    QAngle operator+(const QAngle& v) const {
        return QAngle(x + v.x, y + v.y, z + v.z);
    }

    QAngle operator-(const QAngle& v) const {
        return QAngle(x - v.x, y - v.y, z - v.z);
    }

    QAngle operator*(float fl) const {
        return QAngle(x * fl, y * fl, z * fl);
    }

    QAngle operator/(float fl) const {
        return QAngle(x / fl, y / fl, z / fl);
    }

    QAngle& operator+=(const QAngle& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    QAngle& operator-=(const QAngle& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    QAngle& operator*=(float fl) {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }

    QAngle& operator/=(float fl) {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }

    float Length() const {
        return sqrt(x * x + y * y + z * z);
    }

    QAngle Normalized() const {
        QAngle out = *this;
        out.Normalize();
        return out;
    }

    void Normalize() {
        x = remainderf(x, 360.0f);
        y = remainderf(y, 360.0f);
        z = remainderf(z, 360.0f);
    }

    void Clamp() {
        x = fmaxf(-89.0f, fminf(89.0f, x));
        y = remainderf(y, 360.0f);
        z = 0.0f;
    }

    bool IsZero() const {
        return (x == 0.0f && y == 0.0f && z == 0.0f);
    }
    void ToDirections(Vec3* forward, Vec3* right = nullptr, Vec3* up = nullptr) const {
        float sp, sy, sr;
        float cp, cy, cr;

        float pitch = x * (M_PI / 180.f);
        float yaw = y * (M_PI / 180.f);
        float roll = z * (M_PI / 180.f);

        sp = sinf(pitch);
        cp = cosf(pitch);

        sy = sinf(yaw);
        cy = cosf(yaw);

        sr = sinf(roll);
        cr = cosf(roll);

        if (forward) {
            forward->x = cp * cy;
            forward->y = cp * sy;
            forward->z = -sp;
        }

        if (right) {
            right->x = (-sr * sp * cy) + (-cr * -sy);
            right->y = (-sr * sp * sy) + (-cr * cy);
            right->z = (-sr * cp);
        }

        if (up) {
            up->x = (cr * sp * cy) + (-sr * -sy);
            up->y = (cr * sp * sy) + (-sr * cy);
            up->z = (cr * cp);
        }
    }
};