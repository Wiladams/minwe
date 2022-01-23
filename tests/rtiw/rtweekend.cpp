// Put stuff in here that needs to be instantiated just once
//#define STB_IMAGE_IMPLEMENTATION



#include "rtweekend.h"
#include "interval.h"
#include "aabb.h"
#include "color.h"
#include "vec3.h"

const interval interval::empty = interval(+maths::infinity, -maths::infinity);
const interval interval::universe = interval(-maths::infinity, +maths::infinity);

interval operator+(const interval& ival, double displacement)
{
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival)
{
    return ival + displacement;
}


aabb operator+(const aabb& bbox, const vec3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator+(const vec3& offset, const aabb& bbox) {
    return bbox + offset;
}


PixelRGBA get_color(color pixel_color, int samples_per_pixel)
{
    static const interval intensity(0.000, 0.999);

    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    uint32_t R = static_cast<int>(256 * intensity.clamp(r));
    uint32_t G = static_cast<int>(256 * intensity.clamp(g));
    uint32_t B = static_cast<int>(256 * intensity.clamp(b));

    return PixelRGBA(R, G, B, 255);
}

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel)
{
    auto c = get_color(pixel_color, samples_per_pixel);


    //out << static_cast<int>(256 * intensity.clamp(r)) << ' '
    //<< static_cast<int>(256 * intensity.clamp(g)) << ' '
    //<< static_cast<int>(256 * intensity.clamp(b)) << '\n';
}


// From vec3
inline vec3 random_in_unit_disk()
{
    while (true) {
        auto p = vec3({ maths::random_double(-1, 1), maths::random_double(-1, 1), 0 });
        if (p.lengthSquared() >= 1) 
            continue;

        return p;
    }
}

vec3 random_in_unit_sphere()
{
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.lengthSquared() >= 1) 
            continue;

        return p;
    }
}

vec3 random_unit_vector()
{
    return random_in_unit_sphere().unit();
}

vec3 random_in_hemisphere(const vec3& normal)
{
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.lengthSquared())) * n;
    return r_out_perp + r_out_parallel;
}

vec3 random_cosine_direction()
{
    auto r1 = maths::random_double();
    auto r2 = maths::random_double();

    auto phi = 2 * maths::Pi * r1;
    auto x = cos(phi) * std::sqrt(r2);
    auto y = sin(phi) * std::sqrt(r2);
    auto z = std::sqrt(1 - r2);

    return vec3({ x, y, z });
}
