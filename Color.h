#ifndef COLOR_H
#define COLOR_H
class CColor
{
public:
    CColor();
    CColor(float R, float G, float B);
    bool operator==(const CColor& c);
    float getR() const {return r;}
    float getG() const {return g;}
    float getB() const {return b;}
    void setR(float R) {r = R;}
    void setG(float G) {g = G;}
    void setB(float B) {b = B;}
    void setColor(float R, float G, float B);

    inline CColor operator- (const CColor & c2)
    {
        CColor color;
        color.r = this->r - c2.r;
        color.g = this->g - c2.g;
        color.b = this->b - c2.b;
        return color;
    }
    inline CColor operator+ (const CColor & c2)
    {
        CColor color;
        color.r = this->r + c2.r;
        color.g = this->g + c2.g;
        color.b = this->b + c2.b;
        return color;
    }
    inline void operator+= (const CColor & c2)
    {
        this->r = this->r + c2.r;
        this->g = this->g + c2.g;
        this->b = this->b + c2.b;
    }
    inline CColor operator/ (const float & c2)
    {
        CColor color;
        color.r = this->r / c2;
        color.g = this->g  / c2;
        color.b = this->b  / c2;
        return color;
    }

    static CColor customColor(float R, float G, float B);
private:
    float r,g,b;
};
#endif