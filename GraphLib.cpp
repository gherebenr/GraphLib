#include "Color.h"
#include "GraphLib.h"
#include "PixelBuffer.h"
#include "Shapes.h"
#include <vector>
#include <cmath>
#include <algorithm>

float CGraphLib::iA = 0.16;
float CGraphLib::iL = 1;
float CGraphLib::K = 1;
float CGraphLib::phongConst = 3;
float CGraphLib::kA = 1;
float CGraphLib::kD = 1;
float CGraphLib::kS = 1;
SPoint CGraphLib::lightSource(0.9,0.9,0.9);
bool CGraphLib::halfTone = false;
bool CGraphLib::wire = false;

// Function to move the light source by input amount.
void CGraphLib::moveLight(float x, float y, float z)
{
    lightSource.x += x;
    lightSource.y += y;
    lightSource.z += z;
}

// Set the light source location.
void CGraphLib::setLight(float x, float y, float z)
{
    lightSource.x = x;
    lightSource.y = y;
    lightSource.z = z;
}

// Function that draws a yellow point where the light source is.
void CGraphLib::drawLightSource(std::string plane, int window)
{
    float minCoord = CPixelBuffer::instance(window)->getMinCoord();
    float zoom = CPixelBuffer::instance(window)->getZoom();
    int x,y;
    CColor lightCol(1,1,0);
    if(plane == "xy")
    {
        x = round((lightSource.x-minCoord) * zoom);
        y= round((lightSource.y-minCoord) * zoom);
    }
    else if (plane == "yz")
    {
        x = round((lightSource.y-minCoord) * zoom);
        y= round((lightSource.z-minCoord) * zoom);
    }
    else if (plane == "zx")
    {
        x = round((lightSource.z-minCoord) * zoom);
        y= round((lightSource.x-minCoord) * zoom);
    }
    drawPoint2D(x-1,y,lightCol, window);
    drawPoint2D(x,y,lightCol, window);
    drawPoint2D(x+1,y,lightCol, window);
    drawPoint2D(x+2,y,lightCol, window);
    drawPoint2D(x-1,y+1,lightCol, window);
    drawPoint2D(x,y+1,lightCol, window);
    drawPoint2D(x+1,y+1,lightCol, window);
    drawPoint2D(x+2,y+1,lightCol, window);
    drawPoint2D(x,y+2,lightCol, window);
    drawPoint2D(x+1,y+2,lightCol, window);
    drawPoint2D(x,y-1,lightCol, window);
    drawPoint2D(x+1,y-1,lightCol, window);
}

// Function that draws lines using the DDA algorithm.
void CGraphLib::drawLineDDA(SVertex start, SVertex end, CColor color)
{
    int zoom = CPixelBuffer::instance(0)->getZoom();
    float xD = 0;
    float yD = 0;
    int dx = round(end.x*zoom) - round(start.x*zoom);
    int dy = round(end.y*zoom) - round(start.y*zoom);
    float m = float(dy) / float(dx);
    int xIncr = 0, yIncr = 0;
    if(dx != 0){xIncr = dx / std::abs(dx);}
    if(dy != 0){yIncr = dy / std::abs(dy);}

    drawPoint2D(round(start.x*zoom),round(start.y*zoom),color, 0);
    while((xIncr > 0 && round(start.x*zoom) + xD < round(end.x*zoom)) || (xIncr < 0 && round(start.x*zoom) + xD > round(end.x*zoom)) || (yIncr > 0 && round(start.y*zoom) + yD < round(end.y*zoom)) || (yIncr < 0 && round(start.y*zoom) + yD > round(end.y*zoom)))
    {
        if(std::abs(m) < 1)
        {
            xD += xIncr;
            yD = xD * m;
        }
        else
        {
            yD += yIncr;
            xD = yD * 1/m;
        }
        drawPoint2D(round(start.x*zoom) + xD, round(start.y*zoom) + yD, color, 0);
    }
}

// Function that draws lines using the Bresenham algorithm.
void CGraphLib::drawLineB(SVertex start, SVertex end, CColor color, int pixelBufferIndex)
{
    int zoom = CPixelBuffer::instance(pixelBufferIndex)->getZoom();
    int x = 0, y = 0, yP = 0, xP = 0;
    int sX = round(start.x*zoom);
    int sY = round(start.y*zoom);
    int eX = round(end.x*zoom);
    int eY = round(end.y*zoom);

    int dY = std::abs(eY - sY);
    int dX = std::abs(eX - sX);

    if(dY <= dX)
    {
        int P = 2 * dY - dX;
        
        if(end.x*zoom < start.x*zoom)
        {
            sX = round(end.x*zoom);
            sY = round(end.y*zoom);
            eX = round(start.x*zoom);
            eY = round(start.y*zoom);
        }

        while((sX + x < eX) || (sY < eY && sY + y < eY) || (sY > eY && sY + y > eY))
        {
            drawPoint2D(sX + x, sY + y, color, pixelBufferIndex);
            yP = y;
        
            x++;

            if(P >= 0 && eY > sY)
            {
                y++;
                P = P + 2 * dY - 2 * dX * (y - yP);
            }
            else if(P >= 0 && eY < sY)
            {
                y--;
                P = P + 2 * dY - 2 * dX * (y - yP) * -1;
            }
            else
            {
                P = P + 2 * dY;
            }
        }
    }
    else
    {
        int P = 2 * dX - dY;

        if(end.y*zoom < start.y*zoom)
        {
            sX = round(end.x*zoom);
            sY = round(end.y*zoom);
            eX = round(start.x*zoom);
            eY = round(start.y*zoom);
        }

        while(sY + y < eY || (sX < eX && sX + x < eX) || (sX > eX && sX + x > eX))
        {
            drawPoint2D(sX + x, sY + y, color, pixelBufferIndex);
            xP = x;

            y++;
            
            if(P >= 0 && eX > sX)
            {
                x++;
                P = P + 2 * dX - 2 * dY * (x - xP);
            }
            else if(P >= 0 && eX < sX)
            {
                x--;
                P = P + 2 * dX - 2 * dY * (x - xP) * -1;
            }
            else
            {
                P = P + 2 * dX;
            }
        }
    }
    drawPoint2D(sX,sY,color, pixelBufferIndex);
    drawPoint2D(eX,eY,color, pixelBufferIndex);
}

// Function that sets the color of a pixel in the buffer.
void CGraphLib::drawPoint2D(float x, float y, CColor color, int pixelBufferIndex)
{
    CPixelBuffer::instance(pixelBufferIndex)->setPixelColor(x,y,color,true);
}

// Function that uses a scanline algorithm to fill in a polygon.
void CGraphLib::fillShape2D(CColor color,const SShape shape, int window)
{
    int zoom = CPixelBuffer::instance(0)->getZoom();
    std::vector<int>* triggerPoints = new std::vector<int>[CPixelBuffer::instance(window)->getSizeV()];
    for(int i = 0; i < shape.vertices.size(); i++)
    {
        SVertex vertex = shape.vertices[i];
        SVertex nextV;
        SVertex prevV;
        if(i == shape.vertices.size() - 1){nextV = shape.vertices[0];}
        else {nextV = shape.vertices[i+1];}
        if(i == 0){prevV = shape.vertices[shape.vertices.size()-1];}
        else {prevV = shape.vertices[i-1];}

        int dy = round(nextV.y*zoom) - round(vertex.y*zoom);
        float x = vertex.x*zoom;
        int y = round(vertex.y*zoom);
        float xIncr = 0;
        int yIncr = 0;

        if(dy != 0)
        {
            xIncr = float(round(nextV.x*zoom) - round(vertex.x*zoom)) / float(std::abs(dy));
            yIncr = dy / std::abs(dy);
        }
        
        for(int j = 0; j < std::abs(dy)-1; j++)
        {
            x += xIncr;
            y += yIncr;
            if(y >= 0 && y < CPixelBuffer::instance(window)->getSizeV())
            {
                triggerPoints[y].push_back(int(round(x)));
            }
        }

        if(vertex.y*zoom >= 0 && vertex.y*zoom < CPixelBuffer::instance(window)->getSizeV())
        {
            if((round(prevV.y*zoom) < round(vertex.y*zoom) && round(vertex.y*zoom) < round(nextV.y*zoom)) || (round(prevV.y*zoom) > round(vertex.y*zoom) && round(vertex.y*zoom) > round(nextV.y*zoom)))
            {
                triggerPoints[int(round(vertex.y*zoom))].push_back(round(vertex.x*zoom));
            }
            else if(round(nextV.y*zoom) == round(vertex.y*zoom))
            {
                SVertex nextNextV;
                if(i == shape.vertices.size() - 1){nextNextV = shape.vertices[1];}
                else if(i == shape.vertices.size() - 2){nextNextV = shape.vertices[0];}
                else {nextNextV = shape.vertices[i+2];}
                if((round(prevV.y*zoom) < round(vertex.y*zoom) && round(vertex.y*zoom) < round(nextNextV.y*zoom)) || (round(prevV.y*zoom) > round(vertex.y*zoom) && round(vertex.y*zoom) > round(nextNextV.y*zoom)))
                {
                    triggerPoints[int(round(vertex.y*zoom))].push_back(round(vertex.x*zoom));
                }
            }
        }
    }
    for(int i=0; i < CPixelBuffer::instance(window)->getSizeV(); i++)
    {
        if(triggerPoints[i].size()>0)
        {
            std::sort(triggerPoints[i].begin(), triggerPoints[i].end());
            for(int j = 0; j < triggerPoints[i].size() - 1; j+=2)
            {
                for(int k = triggerPoints[i][j]+1; k < triggerPoints[i][j+1]; k++)
                {
                    drawPoint2D(k,i,color, window);
                }
            }
        }
    }
    delete [] triggerPoints;
}

// Function that returns a color with the highest values for R, G and B currently in the pixel buffer.
CColor CGraphLib::maxColor(const std::vector<STriangle>& allTriangles)
{
    CColor returnColor(0,0,0);
    float maxI = 0;
    for(auto triangle : allTriangles)
    {
        for(int i = 0; i < 3; i++)
        {
            if(triangle.vertices[i].vColor.getR() > maxI){maxI = (triangle.vertices[i].vColor.getR());}
            if(triangle.vertices[i].vColor.getG() > maxI){maxI = (triangle.vertices[i].vColor.getG());}
            if(triangle.vertices[i].vColor.getB() > maxI){maxI = (triangle.vertices[i].vColor.getB());}
        }
    }
    returnColor.setR(maxI);
    returnColor.setG(maxI);
    returnColor.setB(maxI);
    return returnColor;
}

// Function that calculates the color for each vertex of a triangle using Phong's lighting model.
void CGraphLib::calculateTriangleLight(STriangle& triangle,SPoint viewPoint)
{
    float ambient = kA * iA;
    for(int i = 0; i < 3; i++)
    {
        SVertex p = triangle.vertices[i];
        SPoint l = (lightSource - p).normalize();
        SPoint v = (viewPoint - p).normalize();
        SPoint r = (p.normal * 2 * p.normal.dot(l) - l).normalize();
        float temp1 = kD * p.normal.normalize().dot(l);
        float temp2 = kS * pow(r.dot(v), phongConst);
        K = (lightSource - p).length();
        float denom = 1 / ((viewPoint - p).length() + K);
        float ratio = ambient + (temp1 + temp2) * iL * denom;
        triangle.vertices[i].vColor.setR(triangle.defaultColor.getR() * ratio);
        triangle.vertices[i].vColor.setG(triangle.defaultColor.getG() * ratio);
        triangle.vertices[i].vColor.setB(triangle.defaultColor.getB() * ratio);
    }
}

// Function that calls calculateTriangleLight for all triangles, and normalizes the colors.
void CGraphLib::calculateLightAtVertices(std::vector<STriangle>& allTriangles, SPoint viewPoint)
{
    for(auto & ttriangle : allTriangles)
    {
        calculateTriangleLight(ttriangle, viewPoint);
    }
    CColor maxCol = maxColor(allTriangles);
    // CColor maxCol = CColor(1,1,1);
    for(auto & ttriangle : allTriangles)
    {
        for(int i = 0; i < 3; i++)
        {
            ttriangle.vertices[i].vColor.setR(ttriangle.vertices[i].vColor.getR() / maxCol.getR());
            ttriangle.vertices[i].vColor.setG(ttriangle.vertices[i].vColor.getG() / maxCol.getG());
            ttriangle.vertices[i].vColor.setB(ttriangle.vertices[i].vColor.getB() / maxCol.getB());
        }
    }
}

// Function that draws a point for 3D mode.
// If in half-tone mode the pixels are larger, otherwise a normal pixel is drawn.
void CGraphLib::drawPoint3D(float x, float y, CColor color, int pixelBufferIndex)
{
    if (halfTone)
    {
        CColor virtPixCol[9];
        float max = (color.getR() > color.getG()? (color.getR() > color.getB()? color.getR() : color.getB()) : (color.getG()> color.getB()? color.getG(): color.getB()));
        color.setR(color.getR() / max);
        color.setG(color.getG() / max);
        color.setB(color.getB() / max);
        int numOn = ceil(9 * max);
        for(int i = 0; i < 9; i++)
        {
            if(i < numOn)
            {
                virtPixCol[i] = color;
            }
            else
            {
                virtPixCol[i] = CColor(0,0,0);
            }
        }
        drawPoint2D(x, y, virtPixCol[0], pixelBufferIndex);
        drawPoint2D(x+1, y, virtPixCol[1], pixelBufferIndex);
        drawPoint2D(x, y+1, virtPixCol[2], pixelBufferIndex);
        drawPoint2D(x-1, y-1, virtPixCol[3], pixelBufferIndex);
        drawPoint2D(x-1, y, virtPixCol[4], pixelBufferIndex);
        drawPoint2D(x+1, y-1, virtPixCol[5], pixelBufferIndex);
        drawPoint2D(x+1, y+1, virtPixCol[6], pixelBufferIndex);
        drawPoint2D(x-1, y+1, virtPixCol[7], pixelBufferIndex);
        drawPoint2D(x, y-1, virtPixCol[8], pixelBufferIndex);
    }
    else
    {
        drawPoint2D(x,y, color, pixelBufferIndex);
    }
}

// Interpolates the color along the edges of the triangle. Draws the edge of the triangle.
void CGraphLib::edgeInterp(SVertex tVert[], int window, std::string plane, bool active, bool wire)
{
    float minCoord = CPixelBuffer::instance(window)->getMinCoord();
    float zoom = CPixelBuffer::instance(window)->getZoom();
    for(int i = 0; i < 3; i++)
    {
        SVertex a = tVert[i];
        SVertex b = tVert[(i+1) % 3];
        
        int dx, dy;
        if(halfTone)
        {
            dx = round((b.x - minCoord) * zoom / 3) - round((a.x - minCoord) * zoom / 3);
            dy = round((b.y - minCoord) * zoom / 3) - round((a.y - minCoord) * zoom / 3);
        }
        else
        {
            dx = round((b.x - minCoord) * zoom) - round((a.x - minCoord) * zoom);
            dy = round((b.y - minCoord) * zoom) - round((a.y - minCoord) * zoom);
        }
        int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
        float xIncr = float(dx) / float(steps);
        float yIncr = float(dy) / float(steps);
        CColor cIncr = (b.vColor - a.vColor) / float(steps);
        float x,y;
        if(halfTone)
        {
            x = round((a.x - minCoord) * zoom / 3);
            y = round((a.y - minCoord) * zoom / 3);
        }
        else
        {
            x = round((a.x - minCoord) * zoom);
            y = round((a.y - minCoord) * zoom);
        }
        CColor c = a.vColor;
        CColor activeColor = CColor(0,1,0);
        for (int k = 0; k <= steps; k++)
        {
            if(halfTone)
            {
                if(active){
                    drawPoint3D(x * 3 + 1, y * 3 + 1, activeColor, window);
                } else if(wire){
                    drawPoint3D(x * 3 + 1, y * 3 + 1, c, window);
                }
            }
            else
            {
                if(active){
                    drawPoint3D(x, y, activeColor, window);
                } else if(wire){
                    drawPoint3D(x, y, c, window);
                }
                
            }
            x += xIncr;
            y += yIncr;
            c += cIncr;
        }
    }
}

// Interpolates color horizontally, to fill in the triangle.
void CGraphLib::scanLineInterp(int startX, int endX, int y, CColor startC, CColor endC, int window)
{
    int steps = endX - startX;
    if (steps < 0) steps = -steps;
    int xIncr = 1;
    if (endX < startX) xIncr = -1;
    CColor cIncr = (endC - startC) / steps;
    CColor c = startC;
    int x = startX;
    for (int k = 0; k <= steps; k++)
    {
        if(halfTone)
        {
            drawPoint3D(x * 3 + 1, y * 3 + 1, c, window);
        }
        else
        {
            drawPoint3D(x, y, c, window);
        }
        x += xIncr;
        c += cIncr;
    }
}

// Function that draws a triangle.
void CGraphLib::drawTriangle(STriangle& triangle, int window, std::string plane)
{
    float minCoord = CPixelBuffer::instance(window)->getMinCoord();
    float zoom = CPixelBuffer::instance(window)->getZoom();
    SVertex tVert[3];
    for(int i = 0; i < 3; i++)
    {
        if(plane == "xy")
        {
            tVert[i] = triangle.xyProj[i];
        }
        else if (plane == "yz")
        {
            tVert[i] = triangle.yzProj[i];
        }
        else if (plane == "zx")
        {
            tVert[i] = triangle.zxProj[i];
        }
    }

    if(!wire)
    {
        float startX, endX;
        float startXIncr, endXIncr;
        CColor startC, endC;
        CColor startCIncr, endCIncr;
        int startY, endY;
        
        if(halfTone)
        {
            startX = round((tVert[0].x-minCoord) * zoom / 3);
            endX = round((tVert[0].x-minCoord) * zoom / 3);
            startY = round((tVert[0].y-minCoord) * zoom / 3);
            endY = round((tVert[1].y-minCoord) * zoom / 3);
            startXIncr = float(round((tVert[1].x-minCoord) * zoom / 3) - round((tVert[0].x-minCoord) * zoom / 3)) / float(round((tVert[1].y-minCoord) * zoom / 3) - round((tVert[0].y-minCoord) * zoom / 3));
            endXIncr = float(round((tVert[2].x-minCoord) * zoom / 3) - round((tVert[0].x-minCoord) * zoom / 3)) / float(round((tVert[2].y-minCoord) * zoom / 3) - round((tVert[0].y-minCoord) * zoom / 3));
        }
        else
        {
            startX = round((tVert[0].x-minCoord) * zoom);
            endX = round((tVert[0].x-minCoord) * zoom);
            startY = round((tVert[0].y-minCoord) * zoom);
            endY = round((tVert[1].y-minCoord) * zoom);
            startXIncr = float(round((tVert[1].x-minCoord) * zoom) - round((tVert[0].x-minCoord) * zoom)) / float(round((tVert[1].y-minCoord) * zoom) - round((tVert[0].y-minCoord) * zoom));
            endXIncr = float(round((tVert[2].x-minCoord) * zoom) - round((tVert[0].x-minCoord) * zoom)) / float(round((tVert[2].y-minCoord) * zoom) - round((tVert[0].y-minCoord) * zoom));
        }
        startC = tVert[0].vColor;
        endC = tVert[0].vColor;
        startCIncr = (tVert[1].vColor - tVert[0].vColor) / float(round((tVert[1].y-minCoord) * zoom) - round((tVert[0].y-minCoord) * zoom));
        endCIncr = (tVert[2].vColor - tVert[0].vColor) / float(round((tVert[2].y-minCoord) * zoom) - round((tVert[0].y-minCoord) * zoom));
        for (int k = startY; k < endY; k++) 
        {
            scanLineInterp(startX, endX, k, startC, endC, window);
            startX += startXIncr;
            endX += endXIncr;
            startC += startCIncr;
            endC += endCIncr;
        }

        if(halfTone)
        {
            startY = round((tVert[1].y-minCoord) * zoom / 3);
            endY = round((tVert[2].y-minCoord) * zoom / 3);
            startX = round((tVert[1].x-minCoord) * zoom / 3);
            startXIncr = float(round((tVert[2].x-minCoord) * zoom / 3) - round((tVert[1].x-minCoord) * zoom / 3)) / float(round((tVert[2].y-minCoord) * zoom / 3) - round((tVert[1].y-minCoord) * zoom / 3));
        }
        else
        {
            startY = round((tVert[1].y-minCoord) * zoom);
            endY = round((tVert[2].y-minCoord) * zoom);
            startX = round((tVert[1].x-minCoord) * zoom);
            startXIncr = float(round((tVert[2].x-minCoord) * zoom) - round((tVert[1].x-minCoord) * zoom)) / float(round((tVert[2].y-minCoord) * zoom) - round((tVert[1].y-minCoord) * zoom));
        }
        startC = tVert[1].vColor;
        startCIncr = (tVert[2].vColor - tVert[1].vColor) / float(round((tVert[2].y-minCoord) * zoom) - round((tVert[1].y-minCoord) * zoom));
        for (int k = startY; k < endY; k++) 
        {
            scanLineInterp(startX, endX, k, startC, endC, window);
            startX += startXIncr;
            endX += endXIncr;
            startC += startCIncr;
            endC += endCIncr;
        }
    }
    edgeInterp(tVert, window, plane, triangle.active, wire);
}