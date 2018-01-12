#ifndef COLOR_H
#define COLOR_H
class CColor
{
public:
    CColor();
    CColor(float R, float G, float B);
    float getR() const {return r;}
    float getG() const {return g;}
    float getB() const {return b;}
    void setR(float R) {r = R;}
    void setG(float G) {g = G;}
    void setB(float B) {b = B;}
    void setColor(float R, float G, float B);

    // The following functions are used when calculating lighting
    // for each pixel. For example if a pixel has to be half as bright,
    // a new color c2 can be created with values (2,2,2), then
    // by doing c1 / c2, the result will have half the brightness of c1.

    // Subtract one color from another.
    inline CColor operator- (const CColor & c2)
    {
        CColor color;
        color.r = this->r - c2.r;
        color.g = this->g - c2.g;
        color.b = this->b - c2.b;
        return color;
    }
    // Add two colors together.
    inline CColor operator+ (const CColor & c2)
    {
        CColor color;
        color.r = this->r + c2.r;
        color.g = this->g + c2.g;
        color.b = this->b + c2.b;
        return color;
    }
    // Add color to this color, and assign the result to this color.
    inline void operator+= (const CColor & c2)
    {
        this->r = this->r + c2.r;
        this->g = this->g + c2.g;
        this->b = this->b + c2.b;
    }
    // Divide color.
    inline CColor operator/ (const float & c2)
    {
        CColor color;
        color.r = this->r / c2;
        color.g = this->g / c2;
        color.b = this->b / c2;
        return color;
    }
    // Check for equality.
    inline bool operator==(const CColor& c)
    {
        return (this->r == c.r && this->g == c.g && this->b == c.b);
    }
private:
    float r,g,b;
};
#endif 