#include "Color.h"

// Constructors
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

// Function to set values for R, G, and B.
void CColor::setColor(float R, float G, float B)
{
     r = R;
     g = G;
     b = B;
}