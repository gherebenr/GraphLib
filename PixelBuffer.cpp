#include "PixelBuffer.h"
#include "Color.h"
#include "Shapes.h"
#include <cmath>
#include <vector>
#include <string>

std::vector<CPixelBuffer*> CPixelBuffer::pbInstances;

// Pixel buffer factory/getter.
CPixelBuffer *CPixelBuffer::instance(int instanceIndex)
{
    // Create pixel buffers until the index is less than the size
    // of the array of pixel buffers.
    while(pbInstances.size() <= instanceIndex)
    {
        pbInstances.push_back(new CPixelBuffer);
    }
    // Return the pixel buffer with the given index.
    return pbInstances[instanceIndex];
}

// Function that sets the size of the pixel buffer, and the offsets for the viewport.
void CPixelBuffer::setPixelBufferSize(int x, int y, int l, int r, int d, int u, std::string plane)
{
    sizeH = x;
    sizeV = y;

    zoomMult = sizeH;

    offLeft = l;
    offRight = r;
    offDown = d;
    offUp = u;
    
    viewportPlane = plane;

    pixelBuffer = new float[sizeH * sizeV * 3];
}

// Pixel buffer getter.
float *CPixelBuffer::getPixelBuffer() const
{
    return pixelBuffer;
}

// Returns the color of a pixel at a given location.
CColor CPixelBuffer::getPixelColor(int x, int y)
{
    if(x < sizeH && y < sizeV)
    {
        CColor returnColor(*(pixelBuffer + (x + y * sizeH) * 3), *(pixelBuffer + (x + y * sizeH) * 3 + 1), *(pixelBuffer + (x + y * sizeH) * 3 + 2));
        return returnColor;
    }
    return CColor(0,0,0);
}

// Set the color of a pixel at a give location.
void CPixelBuffer::setPixelColor(float xin, float yin, CColor color, bool inView)
{
    int x = round(xin);
    int y = round(yin);
    if(inView && x >= offLeft && x < sizeH - offRight && y >= offDown && y < sizeV - offUp)
    {
        *(pixelBuffer + (x + y * sizeH) * 3) = color.getR();
        *(pixelBuffer + (x + y * sizeH) * 3 + 1) = color.getG();
        *(pixelBuffer + (x + y * sizeH) * 3 + 2) = color.getB();
    }
    else if(!inView && x >= 0 && x < sizeH && y >= 0 && y < sizeV)
    {
        *(pixelBuffer + (x + y * sizeH) * 3) = color.getR();
        *(pixelBuffer + (x + y * sizeH) * 3 + 1) = color.getG();
        *(pixelBuffer + (x + y * sizeH) * 3 + 2) = color.getB();
    }
}

// Resets all pixels to given color.
void CPixelBuffer::resetPixelBuffer(CColor color)
{
    for(int y = 0; y < sizeV; y++)
    {
        for(int x = 0; x < sizeH ; x++)
        {
            if(x >= offLeft && x < sizeH - offRight && y >= offDown && y < sizeV - offUp)
            {
                setPixelColor(x,y,color, true);
            }
            else
            {
                setPixelColor(x,y,CColor(0,0,0), false);
            }
        }
    }
}

// Move the viewport around.
void CPixelBuffer::changeBorder(char border, int amount)
{
    if(border == 'u')
    {
        if(offUpA < 0 && amount > 0)
        {
            offUp += offUpA;
        }
        offUp += amount;
        if(offUp < 0){offUp = 0; offUpA += amount;}
        else{offUpA = 0;}
    }
    else if(border == 'd')
    {
        if(offDownA < 0 && amount > 0)
        {
            offDown += offDownA;
        }
        offDown += amount;
        if(offDown < 0){offDown = 0; offDownA += amount;}
        else{offDownA = 0;}
    }
    else if(border == 'l')
    {
        if(offLeftA < 0 && amount > 0)
        {
            offLeft += offLeftA;
        }
        offLeft += amount;
        if(offLeft < 0){offLeft = 0; offLeftA += amount;}
        else{offLeftA = 0;}
    }
    else if(border == 'r')
    {
        if(offRightA < 0 && amount > 0)
        {
            offRight += offRightA;
        }
        offRight += amount;
        if(offRight < 0){offRight = 0; offRightA += amount;}
        else{offRightA = 0;}
    }
}

// Used for clipping, to know whether one of the vertices of a line is outside the viewport.
bool CPixelBuffer::ablrCode(SVertex vertex, int dir)
{
    int x = round(vertex.x * zoomMult);
    int y = round(vertex.y * zoomMult);
    bool ablr;
    switch(dir)
    {
        case 0: ablr = y >= sizeV - offUp; break;
        case 1: ablr = y < offDown; break;
        case 2: ablr = x < offLeft; break;
        case 3: ablr = x >= sizeH - offRight; break;
    }
    return ablr;
}

// Zoom out to fit all 3D objects.
void CPixelBuffer::changeZoom()
{
    float min = 0;
    float max = 1;
    float zoom = sizeH;
    CShapes::instance()->addActiveToList();
    for(SShape tShape : CShapes::instance()->getAllShapes())
    {
        for(int i = 0; i < tShape.vertices.size(); i++)
        {
            if(tShape.vertices[i].x <= min && (viewportPlane == "xy" || viewportPlane == "zx") )
            {
                min = tShape.vertices[i].x;
            }
            if(tShape.vertices[i].y <= min && (viewportPlane == "xy" || viewportPlane == "yz") )
            {
                min = tShape.vertices[i].y;
            }
            if(tShape.vertices[i].z <= min && (viewportPlane == "yz" || viewportPlane == "zx") )
            {
                min = tShape.vertices[i].z;
            }
            if(tShape.vertices[i].x >= max && (viewportPlane == "xy" || viewportPlane == "zx") )
            {
                max = tShape.vertices[i].x;
            }
            if(tShape.vertices[i].y >= max && (viewportPlane == "xy" || viewportPlane == "yz") )
            {
                max = tShape.vertices[i].y;
            }
            if(tShape.vertices[i].z >= max && (viewportPlane == "yz" || viewportPlane == "zx") )
            {
                max = tShape.vertices[i].z;
            }
        }
    }
    minCoord = min;
    maxCoord = max;
    zoomMult = sizeH * (1 / (max - min));
}