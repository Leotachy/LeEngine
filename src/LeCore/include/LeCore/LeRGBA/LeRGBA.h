
#pragma once


struct LeRGBAReal
{
    float32 red, green, blue, alpha;

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
    uint8 red, green, blue, alpha;

    bool operator == (const LeRGBA& v) const;
    bool operator != (const LeRGBA& v) const;
};