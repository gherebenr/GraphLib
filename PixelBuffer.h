#ifndef PIXBUF_H
#define PIXBUF_H
#include <vector>
#include <string>
#include "Shapes.h"
class CColor;

class CPixelBuffer
{
public:
    static CPixelBuffer *instance(int instanceIndex);
    void setPixelBufferSize(int x, int y, int l, int r, int d, int u, std::string plane);

    void resetPixelBuffer(CColor color);

    int getSizeH() const {return sizeH;}
    int getSizeV() const {return sizeV;}

    float getZoom() const {return zoomMult;}
    float getMinCoord() const {return minCoord;}
    float getMaxCoord() const {return maxCoord;}

    int getTopView() const {return sizeV - offUp - 1;}
    int getBottomView() const {return offDown;}
    int getLeftView() const {return offLeft;}
    int getRightView() const {return sizeH - offRight - 1;}

    CColor getPixelColor(int x, int y);

    void setPixelColor(float x, float y, CColor color, bool inView);

    float *getPixelBuffer() const;

    void changeBorder(char border, int amount);
    bool ablrCode(SVertex vertex, int dir);
    void changeZoom();
private:
    // Vector containing all the pixel buffers.
    static std::vector<CPixelBuffer*> pbInstances;
    // This pixel buffer.
    float *pixelBuffer = nullptr;

    // Size of the viewport.
    int sizeH = 0;
    int sizeV = 0;

    // Offsets for the viewport.
    // The viewport can be smaller than the actual window.
    int offLeft = 0;
    int offRight = 0;
    int offDown = 0;
    int offUp = 0;
    int offLeftA = 0;
    int offRightA = 0;
    int offDownA = 0;
    int offUpA = 0;

    // Used when zooming out to fit all 3D objects.
    // Value to multiply by.
    float zoomMult = sizeH;
    // Smallest and biggest value for a pixel coordinate.
    // Used so the objects stay in the same place when zooming out.
    float minCoord = 0;
    float maxCoord = 1;
   
   // Name of the plane displayed in a window.
   std::string viewportPlane = "";
};
#endif