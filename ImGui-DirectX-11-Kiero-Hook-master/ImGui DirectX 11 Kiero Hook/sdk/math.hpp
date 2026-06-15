#pragma once
#include "../imgui/imgui.h"
#include <cmath>
#include <limits>

#define DEG2RAD(x) ((x) * 3.14159265359f / 180.f)
constexpr double A_PI = 3.14159265358979323846;
constexpr double A_2PI = A_PI * 2.f;
constexpr double A_HALFPI = A_PI / 2.f;

constexpr double A_RAD2DEG = 180.0 / A_PI;
constexpr double A_DEG2RAD = A_PI / 180.0;

constexpr float RAD2DEG(float x) {
    return x * A_RAD2DEG;
}

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
    float Length_2d() const
    {
        return std::sqrtf(this->x * this->x + this->y * this->y);
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
    float normalize()
    {
        float length = this->Length();

        if (length != 0.0f) {
            this->x /= length + std::numeric_limits< float >::epsilon();
            this->y /= length + std::numeric_limits< float >::epsilon();
            this->z /= length + std::numeric_limits< float >::epsilon();
        }

        return length;
    }
    [[nodiscard]] Vec3 Normalize() const
    {
        const float flLength = this->Length();
        return *this / flLength;
    }
    Vec3 cross(const Vec3& other) const
    {
        return { this->y * other.z - this->z * other.y, this->z * other.x - this->x * other.z,
                 this->x * other.y - this->y * other.x };
    }
    Vec3 normalized() const
    {
        auto res = *this;
        auto l = res.Length();

        if (l != 0.0f)
            res /= l;
        else
            res.x = res.y = res.z = 0.0f;

        return res;
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

    float dot(const Vec3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    float dist_to_sq(const Vec3& v) const
    {
        const float dx = x - v.x;
        const float dy = y - v.y;
        const float dz = z - v.z;

        return dx * dx + dy * dy + dz * dz;
    }

    float dist_to(const Vec3& v) const
    {
        return std::sqrt(dist_to_sq(v));
    }

    void to_directions(Vec3* vec_forward, Vec3* vec_right, Vec3* vec_up) const {
        float pitch_rad = this->x * 0.0174532925f;
        float yaw_rad = this->y * 0.0174532925f;
        float roll_rad = this->z * 0.0174532925f;

        float sp = std::sin(pitch_rad), cp = std::cos(pitch_rad);
        float sy = std::sin(yaw_rad), cy = std::cos(yaw_rad);
        float sr = std::sin(roll_rad), cr = std::cos(roll_rad);

        if (vec_forward != nullptr) {
            vec_forward->x = cp * cy;
            vec_forward->y = cp * sy;
            vec_forward->z = -sp;
        }

        if (vec_right != nullptr) {
            vec_right->x = (-sr * sp * cy) + (-cr * -sy);
            vec_right->y = (-sr * sp * sy) + (-cr * cy);
            vec_right->z = -sr * cp;
        }

        if (vec_up != nullptr) {
            vec_up->x = (cr * sp * cy) + (-sr * -sy);
            vec_up->y = (cr * sp * sy) + (-sr * cy);
            vec_up->z = cr * cp;
        }
    }

    Vec3 operator+(const Vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
    Vec3 operator-(const Vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
    Vec3 operator*(float f) const { return { x * f, y * f, z * f }; }
    Vec3 operator/(float f) const { return { x / f, y / f, z / f }; }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    Vec3& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }

    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec3& v) const { return x != v.x || y != v.y || z != v.z; }

    Vec3 operator-() const { return { -x, -y, -z }; }
};

struct Vec4
{
    float x, y, z, w;

    constexpr Vec4(float x = 0.f,
        float y = 0.f,
        float z = 0.f,
        float w = 0.f) noexcept
        : x(x), y(y), z(z), w(w) {
    }

    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    float LengthSqr() const
    {
        return x * x + y * y + z * z + w * w;
    }

    bool IsZero(float eps = 0.0001f) const
    {
        return std::abs(x) < eps &&
            std::abs(y) < eps &&
            std::abs(z) < eps &&
            std::abs(w) < eps;
    }

    Vec4 Normalize() const
    {
        const float len = Length();
        return *this / len;
    }

    float NormalizeInPlace()
    {
        const float len = Length();
        const float inv = 1.0f / (len + std::numeric_limits<float>::epsilon());

        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;

        return len;
    }
    Vec3 rotate_vector(const Vec3& v) const {

        Vec4 qv(v.x, v.y, v.z, 0.0f);

        Vec4 q_conj(-x, -y, -z, w);

        Vec4 temp;
        temp.w = w * qv.w - x * qv.x - y * qv.y - z * qv.z;
        temp.x = w * qv.x + x * qv.w + y * qv.z - z * qv.y;
        temp.y = w * qv.y - x * qv.z + y * qv.w + z * qv.x;
        temp.z = w * qv.z + x * qv.y - y * qv.x + z * qv.w;

        Vec4 result;
        result.w = temp.w * q_conj.w - temp.x * q_conj.x - temp.y * q_conj.y - temp.z * q_conj.z;
        result.x = temp.w * q_conj.x + temp.x * q_conj.w + temp.y * q_conj.z - temp.z * q_conj.y;
        result.y = temp.w * q_conj.y - temp.x * q_conj.z + temp.y * q_conj.w + temp.z * q_conj.x;
        result.z = temp.w * q_conj.z + temp.x * q_conj.y - temp.y * q_conj.x + temp.z * q_conj.w;

        return Vec3(result.x, result.y, result.z);
    }
    Vec4 operator+(const Vec4& v) const
    {
        return { x + v.x, y + v.y, z + v.z, w + v.w };
    }

    Vec4 operator-(const Vec4& v) const
    {
        return { x - v.x, y - v.y, z - v.z, w - v.w };
    }

    Vec4 operator*(float f) const
    {
        return { x * f, y * f, z * f, w * f };
    }

    Vec4 operator/(float f) const
    {
        return { x / f, y / f, z / f, w / f };
    }
};
struct Matrix3x4;
struct bone_data_t
{
    Vec3 m_pos;
    float m_scale;
    Vec4 m_rot;

    Matrix3x4 to_matrix() const;
};

struct Matrix2x4
{
    float m[2][4];

    constexpr Matrix2x4() noexcept
        : m{
            {1.f, 0.f, 0.f, 0.f},
            {0.f, 1.f, 0.f, 0.f}
        }
    {
    }

    constexpr Matrix2x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13) noexcept
        : m{
            {m00, m01, m02, m03},
            {m10, m11, m12, m13}
        }
    {
    }

    float* operator[](int i) { return m[i]; }
    const float* operator[](int i) const { return m[i]; }
};

struct Matrix3x4
{
    float m[3][4];

    constexpr Matrix3x4() noexcept
        : m{
            {1.f, 0.f, 0.f, 0.f},
            {0.f, 1.f, 0.f, 0.f},
            {0.f, 0.f, 1.f, 0.f}
        }
    {
    }

    constexpr Matrix3x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23) noexcept
        : m{
            {m00, m01, m02, m03},
            {m10, m11, m12, m13},
            {m20, m21, m22, m23}
        }
    {
    }

    float* operator[](int i)
    {
        return m[i];
    }

    const float* operator[](int i) const
    {
        return m[i];
    }

    Vec3 GetOrigin() const
    {
        return {
            m[0][3],
            m[1][3],
            m[2][3]
        };
    }

    void SetOrigin(const Vec3& origin)
    {
        m[0][3] = origin.x;
        m[1][3] = origin.y;
        m[2][3] = origin.z;
    }

    Vec3 Transform(const Vec3& v) const
    {
        return {
            v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3],
            v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3],
            v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3]
        };
    }
    
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

inline float normalize_float(float value)
{
    while (value > 180.f)
        value -= 360.f;

    while (value < -180.f)
        value += 360.f;

    return value;
}
