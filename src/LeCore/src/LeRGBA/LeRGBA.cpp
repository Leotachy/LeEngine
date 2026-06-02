
#include "LeCore/LeCore.h"
#include "LeCore/LeRGBA/LeRGBA.h"


LeRGBAReal::LeRGBAReal()
{
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
}

LeRGBAReal::LeRGBAReal(float32 r, float32 g, float32 b, float32 a)
{
    r = glm::clamp<float32>(r, 0.0f, 1.0f);
    g = glm::clamp<float32>(g, 0.0f, 1.0f);
    b = glm::clamp<float32>(b, 0.0f, 1.0f);
    a = glm::clamp<float32>(a, 0.0f, 1.0f);

    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

LeRGBAReal& LeRGBAReal::operator += (const LeRGBAReal& v)
{
    r += v.r;
    g += v.g;
    b += v.b;
    a += v.a;
    return *this;
}

LeRGBAReal& LeRGBAReal::operator -= (const LeRGBAReal& v)
{
    r -= v.r;
    g -= v.g;
    b -= v.b;
    a -= v.a;
    return *this;
}

LeRGBAReal& LeRGBAReal::operator *= (float32 d)
{
    r *= d;
    g *= d;
    b *= d;
    a *= d;
    return *this;
}

LeRGBAReal& LeRGBAReal::operator /= (float32 d)
{
    r /= d;
    g /= d;
    b /= d;
    a /= d;
    return *this;
}

LeRGBAReal LeRGBAReal::operator + (const LeRGBAReal& v) const
{
    return { r + v.r, g + v.g, b + v.b, a + v.a };
}

LeRGBAReal LeRGBAReal::operator - (const LeRGBAReal& v) const
{
    return { r - v.r, g - v.g, b - v.b, a - v.a };
}

LeRGBAReal LeRGBAReal::operator * (float32 d) const
{
    return { r * d, g * d, b * d, a * d };
}

LeRGBAReal LeRGBAReal::operator / (float32 d) const
{
    return { r / d, g / d, b / d, a / d };
}

bool LeRGBAReal::operator == (const LeRGBAReal& v) const
{
    return r == v.r &&
        g == v.g &&
        b == v.b &&
        a == v.a;
}

bool LeRGBAReal::operator != (const LeRGBAReal& v) const
{
    return !(*this == v);
}




LeRGBA::LeRGBA()
{
    this->rgba = 0xFFFFFFFF;
}
LeRGBA::LeRGBA(const uint8 r, const uint8 g, const uint8 b, const uint8 a)
{
    this->r = r; this->g = g; this->b = b; this->a = a;
}
LeRGBA::LeRGBA(const uint32 rgba)
{
    this->rgba = rgba;
}

LeRGBAReal LeRGBA::ToRgbaReal(void) const
{
    return { float32(r) / 0xFF, float32(g) / 0xFF , float32(b) / 0xFF , float32(a) / 0xFF };
}

bool LeRGBA::operator == (const LeRGBA& v) const
{
    return r == v.r &&
        g == v.g &&
        b == v.b &&
        a == v.a;
}

bool LeRGBA::operator != (const LeRGBA& v) const
{
    return !(*this == v);
}