
#pragma once


struct LeRGBAReal
{
    float32 r, g, b, a;

    LeRGBAReal();
    LeRGBAReal(float32 r, float32 g, float32 b, float32 a);

    LeRGBAReal& operator += (const LeRGBAReal& v);
    LeRGBAReal& operator -= (const LeRGBAReal& v);
    LeRGBAReal& operator *= (float32 d);
    LeRGBAReal& operator /= (float32 d);

    LeRGBAReal operator + (const LeRGBAReal& v) const;
    LeRGBAReal operator - (const LeRGBAReal& v) const;
    LeRGBAReal operator * (float32 d) const;
    LeRGBAReal operator / (float32 d) const;

    bool operator == (const LeRGBAReal& v) const;
    bool operator != (const LeRGBAReal& v) const;
};


struct LeRGBA
{
    union
    {
        struct
        {
            uint8 r, g, b, a;
        };

        uint32 rgba;
    };

    LeRGBA();
    LeRGBA(const uint8 r, const uint8 g, const uint8 b, const uint8 a);
    LeRGBA(const uint32 rgba);

    LeRGBAReal ToRgbaReal(void) const;

    bool operator == (const LeRGBA& v) const;
    bool operator != (const LeRGBA& v) const;
};