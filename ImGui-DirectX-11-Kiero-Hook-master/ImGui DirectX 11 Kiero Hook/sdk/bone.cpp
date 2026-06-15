#include "bone.hpp"
#include <algorithm>

#undef min
#undef max




Matrix3x4 bone_data_t::to_matrix() const {
    Matrix3x4 matrix;

    const Vec4& q = m_rot;
    const Vec3& pos = m_pos;

    const float xx = q.x * q.x;
    const float yy = q.y * q.y;
    const float zz = q.z * q.z;
    const float xy = q.x * q.y;
    const float xz = q.x * q.z;
    const float yz = q.y * q.z;
    const float wx = q.w * q.x;
    const float wy = q.w * q.y;
    const float wz = q.w * q.z;

    matrix[0][0] = 1.0f - 2.0f * (yy + zz);
    matrix[0][1] = 2.0f * (xy - wz);
    matrix[0][2] = 2.0f * (xz + wy);
    matrix[0][3] = pos.x;

    matrix[1][0] = 2.0f * (xy + wz);
    matrix[1][1] = 1.0f - 2.0f * (xx + zz);
    matrix[1][2] = 2.0f * (yz - wx);
    matrix[1][3] = pos.y;

    matrix[2][0] = 2.0f * (xz - wy);
    matrix[2][1] = 2.0f * (yz + wx);
    matrix[2][2] = 1.0f - 2.0f * (xx + yy);
    matrix[2][3] = pos.z;

    return matrix;
}

bool c_hitbox_data::segment_intersects_capsule(const Vec3& start, const Vec3& end) {
    if (m_radius <= 0.0f) {
        Vec3 seg_min = { std::min(start.x, end.x), std::min(start.y, end.y), std::min(start.z, end.z) };
        Vec3 seg_max = { std::max(start.x, end.x), std::max(start.y, end.y), std::max(start.z, end.z) };

        if (m_maxs.x < seg_min.x || m_mins.x > seg_max.x ||
            m_maxs.y < seg_min.y || m_mins.y > seg_max.y ||
            m_maxs.z < seg_min.z || m_mins.z > seg_max.z) {
            return false;
        }
        return true;
    }

    if ((m_maxs - m_mins).Length() < 0.1f) {
        Vec3 center = (m_mins + m_maxs) * 0.5f;

        Vec3 line_dir = end - start;
        float line_len = line_dir.Length();
        if (line_len < 0.001f) return center.dist_to(start) <= m_radius;

        line_dir /= line_len;
        Vec3 to_center = center - start;

        float proj = to_center.dot(line_dir);
        if (proj < 0.0f) proj = 0.0f;
        if (proj > line_len) proj = line_len;

        Vec3 closest = start + line_dir * proj;
        float dist_sq = center.dist_to_sq(closest);

        return dist_sq <= (m_radius * m_radius);
    }

    Vec3 v = m_maxs - m_mins;
    Vec3 line_dir = end - start;
    float line_len = line_dir.Length();
    if (line_len < 0.001f) {
        Vec3 w = start - m_mins;
        float c1 = w.dot(v);
        if (c1 <= 0.0f) return start.dist_to_sq(m_mins) <= (m_radius * m_radius);
        float c2 = v.dot(v);
        if (c2 <= c1) return start.dist_to_sq(m_maxs) <= (m_radius * m_radius);
        float b = c1 / c2;
        Vec3 pb = m_mins + v * b;
        return start.dist_to_sq(pb) <= (m_radius * m_radius);
    }

    line_dir /= line_len;

    Vec3 u = v / v.Length();
    float capsule_len = v.Length();

    float denom = line_dir.dot(u);
    Vec3 w0 = start - m_mins;

    float a = line_dir.dot(line_dir) - denom * denom;
    float b = 2.0f * (line_dir.dot(w0) - denom * (w0.dot(u)));
    float c = w0.dot(w0) - m_radius * m_radius - w0.dot(u) * w0.dot(u);

    if (a < 0.001f) {
        float t = -c / b;
        if (t >= 0.0f && t <= line_len) {
            Vec3 point_on_line = start + line_dir * t;
            Vec3 point_on_capsule = m_mins + u * (w0.dot(u) + t * denom);
            point_on_capsule.x = (point_on_capsule.x < m_mins.x) ? m_mins.x : (point_on_capsule.x > m_maxs.x ? m_maxs.x : point_on_capsule.x);
            point_on_capsule.y = (point_on_capsule.y < m_mins.y) ? m_mins.y : (point_on_capsule.y > m_maxs.y ? m_maxs.y : point_on_capsule.y);
            point_on_capsule.z = (point_on_capsule.z < m_mins.z) ? m_mins.z : (point_on_capsule.z > m_maxs.z ? m_maxs.z : point_on_capsule.z);
            return point_on_line.dist_to_sq(point_on_capsule) <= (m_radius * m_radius);
        }
        return false;
    }

    float discr = b * b - 4.0f * a * c;
    if (discr < 0.0f) return false;

    float sqrt_discr = sqrtf(discr);
    float t1 = (-b - sqrt_discr) / (2.0f * a);
    float t2 = (-b + sqrt_discr) / (2.0f * a);

    if (t1 > line_len && t2 > line_len) return false;
    if (t1 < 0.0f && t2 < 0.0f) return false;

    t1 = (t1 < 0.0f) ? 0.0f : (t1 > line_len ? line_len : t1);
    t2 = (t2 < 0.0f) ? 0.0f : (t2 > line_len ? line_len : t2);

    for (float t : {t1, t2}) {
        if (t >= 0.0f && t <= line_len) {
            Vec3 point_on_line = start + line_dir * t;
            float proj_on_capsule = w0.dot(u) + t * denom;
            proj_on_capsule = (proj_on_capsule < 0.0f) ? 0.0f : (proj_on_capsule > capsule_len ? capsule_len : proj_on_capsule);
            Vec3 point_on_capsule = m_mins + u * proj_on_capsule;

            if (point_on_line.dist_to_sq(point_on_capsule) <= (m_radius * m_radius)) {
                return true;
            }
        }
    }

    return false;
}


float c_hitbox_data::projected_valid_radius(const Vec3& view_point, const Vec3& point_on_capsule) {

    const Vec3 vec_to_point = point_on_capsule - m_mins;
    float t = vec_to_point.dot(m_dir);
    t = std::clamp(t, 0.0f, m_axis_len);

    const Vec3 closest_point_on_axis = m_mins + m_dir * t;
    Vec3 radial = point_on_capsule - closest_point_on_axis;

    float radial_length_sqr = radial.LengthSqr();
    if (radial_length_sqr < 1e-6f) {
        return 0.0f;
    }

    Vec3 view_dir = (point_on_capsule - view_point);
    float view_distance_sqr = view_dir.LengthSqr();

    if (view_distance_sqr < 1e-6f) {
        return m_radius;
    }

    view_dir = view_dir.Normalize();
    Vec3 radial_normalized = radial.Normalize();

    Vec3 cross_product = radial_normalized.cross(view_dir);
    float sin_angle = cross_product.Length();

    return std::min(m_radius * sin_angle, m_radius);
}