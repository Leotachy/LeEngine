
#include "LeCore/LeCore.h"
#include "LeCore/LeRGBA/LeRGBA.h"

LeRGBAReal& LeRGBAReal::operator += (const LeRGBAReal& v)
{
    red += v.red;
    green += v.green;
    blue += v.blue;
    alpha += v.alpha;
    return *this;
}

LeRGBAReal& LeRGBAReal::operator -= (const LeRGBAReal& v)
{
    red -= v.red;
    green -= v.green;
    blue -= v.blue;
    alpha -= v.alpha;
    return *this;
}

LeRGBAReal& LeRGBAReal::operator *= (float32 d)
{
    red *= d;
    green *= d;
    blue *= d;
    alpha *= d;
    return *this;
}

LeRGBAReal& LeRGBAReal::operator /= (float32 d)
{
    red /= d;
    green /= d;
    blue /= d;
    alpha /= d;
    return *this;
}

LeRGBAReal LeRGBAReal::operator + (const LeRGBAReal& v) const
{
    return { red + v.red, green + v.green, blue + v.blue, alpha + v.alpha };
}

LeRGBAReal LeRGBAReal::operator - (const LeRGBAReal& v) const
{
    return { red - v.red, green - v.green, blue - v.blue, alpha - v.alpha };
}

LeRGBAReal LeRGBAReal::operator * (float32 d) const
{
    return { red * d, green * d, blue * d, alpha * d };
}

LeRGBAReal LeRGBAReal::operator / (float32 d) const
{
    return { red / d, green / d, blue / d, alpha / d };
}

bool LeRGBAReal::operator == (const LeRGBAReal& v) const
{
    return red == v.red &&
        green == v.green &&
        blue == v.blue &&
        alpha == v.alpha;
}

bool LeRGBAReal::operator != (const LeRGBAReal& v) const
{
    return !(*this == v);
}





bool LeRGBA::operator == (const LeRGBA& v) const
{
    return red == v.red &&
        green == v.green &&
        blue == v.blue &&
        alpha == v.alpha;
}

bool LeRGBA::operator != (const LeRGBA& v) const
{
    return !(*this == v);
}