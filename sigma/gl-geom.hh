#ifndef σgl_geom_h
#define σgl_geom_h

#include <GLES2/gl2.h>

#include "../sigma-pre.hh"
#include "begin.hh"

namespace σ
{


struct color;
struct vec2;
struct vec3;
struct vec4;
struct mat4;

typedef color const colorc;
typedef vec2  const vec2c;
typedef vec3  const vec3c;
typedef vec4  const vec4c;
typedef mat4  const mat4c;


struct color
{
  using f64 = τ::f64;
  using u32 = τ::u32;

  f64 r, g, b, a;

  color() = default;
  color(f64 r, f64 g, f64 b, f64 a)     : r(r), g(g), b(b), a(a) {}
  color(f64 r, f64 g, f64 b)            : r(r), g(g), b(b), a(1) {}
  color(f64 r)                          : r(r), g(r), b(r), a(1) {}
  color(τ::T<f64, f64, f64, f64> const &t) : r(std::get<0>(t)), g(std::get<1>(t)), b(std::get<2>(t)), a(std::get<3>(t)) {}
  color(τ::ηic &x)                         : r(x[0].cf()), g(x[1].cf()), b(x[2].cf()), a(x[3].cf()) {}

  color blend(color const &o) const
  {
    return {r * (1 - o.a) + o.r * o.a,
            g * (1 - o.a) + o.g * o.a,
            b * (1 - o.a) + o.b * o.a,
            a * (1 - o.a) + o.a * o.a};
  }

  color interpolate(color const &o, f64 t) const
  {
    return {r * (1 - t) + o.r * t,
            g * (1 - t) + o.g * t,
            b * (1 - t) + o.b * t,
            a * (1 - t) + o.a * t};
  }

  u32 rgba() const
    {
      return u32(r * 255) << 24
           | u32(g * 255) << 16
           | u32(b * 255) << 8
           | u32(a * 255);
    }

  τ::St hex() const
    {
      return (τ::Ss{} << "rgba("
              << int(r * 255) << ","
              << int(g * 255) << ","
              << int(b * 255) << "," << a << ")").str();
    }
};


struct vec2 final
{
  using f64 = τ::f64;

  f64 x, y;

  vec2() = default;
  vec2(f64 x, f64 y) : x(x), y(y) {}
  vec2(f64 x) : x(x), y(x) {}
  vec2(τ::ηic &i) : x(i[0].cf()), y(i[1].cf()) {}

  explicit operator bool() const { return x != 0 || y != 0; }

  vec2 operator+(vec2 const &o) const { return {x + o.x, y + o.y}; }
  vec2 operator-(vec2 const &o) const { return {x - o.x, y - o.y}; }
  vec2 operator*(vec2 const &o) const { return {x * o.x, y * o.y}; }
  vec2 operator/(vec2 const &o) const { return {x / o.x, y / o.y}; }

  vec2 operator+(f64 o) const { return {x + o, y + o}; }
  vec2 operator-(f64 o) const { return {x - o, y - o}; }
  vec2 operator*(f64 o) const { return {x * o, y * o}; }
  vec2 operator/(f64 o) const { return {x / o, y / o}; }

  f64  dot (vec2 const &o) const { return x * o.x + y * o.y; }
  vec2 proj(vec2 const &o) const { return o * (dot(o) / o.dot(o)); }
  vec2 orth(vec2 const &o) const { return *this - proj(o); }

  vec2 rotate(f64 angle) const
  {
    return {x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle)};
  }

  vec2 interpolate(vec2 const &o, f64 t) const
  {
    return {x * (1 - t) + o.x * t, y * (1 - t) + o.y * t};
  }
};


struct vec3 final
{
  using f64 = τ::f64;

  f64 x, y, z;

  vec3() = default;
  vec3(f64 x, f64 y, f64 z) : x(x), y(y), z(z) {}
  vec3(f64 x) : x(x), y(x), z(x) {}
  vec3(τ::ηic &i) : x(i[0].cf()), y(i[1].cf()), z(i[2].cf()) {}

  explicit operator bool() const { return x != 0 || y != 0 || z != 0; }

  vec3 operator+(vec3 const &o) const { return {x + o.x, y + o.y, z + o.z}; }
  vec3 operator-(vec3 const &o) const { return {x - o.x, y - o.y, z - o.z}; }
  vec3 operator*(vec3 const &o) const { return {x * o.x, y * o.y, z * o.z}; }
  vec3 operator/(vec3 const &o) const { return {x / o.x, y / o.y, z / o.z}; }

  vec3 operator+(f64 o) const { return {x + o, y + o, z + o}; }
  vec3 operator-(f64 o) const { return {x - o, y - o, z - o}; }
  vec3 operator*(f64 o) const { return {x * o, y * o, z * o}; }
  vec3 operator/(f64 o) const { return {x / o, y / o, z / o}; }

  f64  dot  (vec3 const &o) const { return x * o.x + y * o.y + z * o.z; }
  vec3 proj (vec3 const &o) const { return o * (dot(o) / o.dot(o)); }
  vec3 orth (vec3 const &o) const { return *this - proj(o); }
  vec3 cross(vec3 const &o) const
  {
    return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
  }

  f64  len()  const { return sqrt(dot(*this)); }
  vec3 norm() const { return *this / len(); }

  vec3 rotate(vec3 const &axis, f64 angle) const
  {
    return *this * cos(angle) + axis.cross(*this) * sin(angle) + axis * axis.dot(*this) * (1 - cos(angle));
  }

  vec3 interpolate(vec3 const &o, f64 t) const
  {
    return {x * (1 - t) + o.x * t, y * (1 - t) + o.y * t, z * (1 - t) + o.z * t};
  }
};


struct vec4 final
{
  using f64 = τ::f64;

  f64 x, y, z, w;

  vec4() = default;
  vec4(f64 x, f64 y, f64 z, f64 w) : x(x), y(y), z(z), w(w) {}
  vec4(f64 x) : x(x), y(x), z(x), w(x) {}
  vec4(vec3 const &v, f64 w) : x(v.x), y(v.y), z(v.z), w(w) {}
  vec4(τ::ηic &i) : x(i[0].cf()), y(i[1].cf()), z(i[2].cf()), w(i[3].cf()) {}

  explicit operator bool() const { return x != 0 || y != 0 || z != 0 || w != 0; }

  vec4 operator+(vec4 const &o) const { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
  vec4 operator-(vec4 const &o) const { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
  vec4 operator*(vec4 const &o) const { return {x * o.x, y * o.y, z * o.z, w * o.w}; }
  vec4 operator/(vec4 const &o) const { return {x / o.x, y / o.y, z / o.z, w / o.w}; }

  vec4 operator+(f64 o) const { return {x + o, y + o, z + o, w + o}; }
  vec4 operator-(f64 o) const { return {x - o, y - o, z - o, w - o}; }
  vec4 operator*(f64 o) const { return {x * o, y * o, z * o, w * o}; }
  vec4 operator/(f64 o) const { return {x / o, y / o, z / o, w / o}; }

  f64 dot(vec4 const &o) const { return x * o.x + y * o.y + z * o.z + w * o.w; }

  f64  len()               const { return sqrt(dot(*this)); }
  vec4 norm()              const { return *this / len(); }
  vec4 proj(vec4 const &o) const { return o * (dot(o) / o.dot(o)); }
  vec4 orth(vec4 const &o) const { return *this - proj(o); }

  vec4 interpolate(vec4 const &o, f64 t) const
  {
    return {x * (1 - t) + o.x * t, y * (1 - t) + o.y * t, z * (1 - t) + o.z * t, w * (1 - t) + o.w * t};
  }
};


struct mat4 final
{
  using f64 = τ::f64;

  f64 m[4][4];

# define for_ij                                 \
  for (int i = 0; i < 4; ++i)                   \
    for (int j = 0; j < 4; ++j)

  mat4() = default;
  mat4(const mat4& x) { memcpy(m, x.m, sizeof(f64) * 16); }
  mat4(f64 x)         { for_ij m[i][j] = x; }

  mat4(f64 m00, f64 m01, f64 m02, f64 m03,
       f64 m10, f64 m11, f64 m12, f64 m13,
       f64 m20, f64 m21, f64 m22, f64 m23,
       f64 m30, f64 m31, f64 m32, f64 m33)
    {
      m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
      m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
      m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
      m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

  mat4 operator+(const mat4& rhs) const { mat4 r; for_ij r.m[i][j] = m[i][j] + rhs.m[i][j]; return r; }
  mat4 operator-(const mat4& rhs) const { mat4 r; for_ij r.m[i][j] = m[i][j] - rhs.m[i][j]; return r; }
  mat4 operator*(f64 x)           const { mat4 r; for_ij r.m[i][j] = m[i][j] * x; return r; }
  mat4 operator*(mat4 const &rhs) const
  {
    mat4 r(0);
    for_ij
      for (int k = 0; k < 4; k++)
        r.m[i][j] += m[i][k] * rhs.m[k][j];
    return r;
  }

  mat4 transpose() const { mat4 r; for_ij r.m[i][j] = m[j][i]; return r; }
  mat4 inv()       const;
  f64  det()       const;

  static mat4 identity();
  static mat4 translate(f64 x, f64 y, f64 z);
  static mat4 scale(f64 x, f64 y, f64 z);
  static mat4 rotate_x(f64 r);
  static mat4 rotate_y(f64 r);
  static mat4 rotate_z(f64 r);

# undef for_ij
};


τ::O &operator<<(τ::O &s, color const &c);
τ::O &operator<<(τ::O &s, vec2 const &v);
τ::O &operator<<(τ::O &s, vec3 const &v);
τ::O &operator<<(τ::O &s, mat4 const &v);


}

#include "end.hh"

#endif
