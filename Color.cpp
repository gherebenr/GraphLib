#include "Color.h"

CColor::CColor()
{
     r = g = b = 0;
}
CColor::CColor(float R, float G, float B)
{
     r = R;
     g = G;
     b = B;
}

void CColor::setColor(float R, float G, float B)
{
     r = R;
     g = G;
     b = B;
}

bool CColor::operator==(const CColor& c)
{
    return (this->r == c.r && this->g == c.g && this->b == c.b);
}

CColor CColor::customColor(float R, float G, float B)
{
    CColor color(R,G,B);
    return color;
}