#include <GL/glut.h>
#include <vector>
#include "Color.h"
#include "PixelBuffer.h"
#include "GraphLib.h"
#include "FileIO.h"
#include "Shapes.h"
#include <iostream>
#include <string>
#include <cmath>
#include <string.h>
#include <limits>

bool _3Dmode = false;
bool curveEditor = false;
bool wire = false;
bool terminalMode = false;

bool useDDA = false;
bool bFill = true;
bool drawOutline = true;

bool translate = true;
bool rotate = false;
bool scale = false;
bool viewportMove = false;
bool borderSizeChange = false;
bool rotationLineInput = false;
bool lightSource = false;

int winSizeH = 500;
int winSizeV = 500;

int MainWindow;
int xyWindow;
int zxWindow;
int yzWindow;

bool modifyingVertex = false;
int indexOfVertexToModify = -1;

int curveOrder = 2;
int curveResolution = 1000;

CColor backgroundColor (.2,.2,.2);

SPoint viewPointXY(0.5, 0.5, 5);
SPoint viewPointYZ(5, 0.5, 0.5);
SPoint viewPointZX(0.5, 5, 0.5);

SVertex rotStartPt;
SVertex rotEndPt;
 float rotDegree = 0;

void display2D();
void mouseClick2D(int button, int state, int x, int y);
void mouseDragVertex(int x, int y);
void keyActions2D(unsigned char key, int x, int y);
void specialInput2D(int key, int x, int y);

void refreshAllWindows();
void display3DXY();
void display3DZX();
void display3DYZ();
void keyActions3D(unsigned char key, int x, int y);
void specialInput3D(int key, int x, int y);

void consoleInput();
void timer(int val);

int main(int argc, char *argv[])
{
    std::string userInput = "";
    
    std::cout << "Currently in 2D mode. Do you want to switch to 3D mode? y/n: ";
    std::cin >> userInput;
    if(userInput == "y" || userInput == "Y")
    {
        std::cout << "3D mode selected" << std::endl;
        _3Dmode = true;
        curveEditor = false;
        std::cout << "Do you want to use the terminal to input commands? y/n: ";
        std::cin >> userInput;
        if(userInput == "y" || userInput == "Y")
        {
            terminalMode = true;
        }
    }
    else
    {
        std::cout << "2D mode selected" << std::endl;
        _3Dmode = false;
        curveEditor = false;
        std::cout << "Currently in polygon mode. Do you want to switch to curve mode? y/n: ";
        std::cin >> userInput;
        if(userInput == "y" || userInput == "Y")
        {
            curveEditor = true;
        }
    }

    if(!_3Dmode && argc > 2)
    {
        winSizeH = atoi(argv[1]);
        winSizeV = atoi(argv[2]);
    }
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(winSizeH, winSizeV);
    
    if(!_3Dmode)
    {
        glutInitWindowPosition(0, 0);
        MainWindow = glutCreateWindow("2D Mode");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display2D);
        glutMouseFunc(mouseClick2D);
        glutMotionFunc(mouseDragVertex);
        glutKeyboardFunc(keyActions2D);
        glutSpecialFunc(specialInput2D);
        CPixelBuffer::instance(0)->setPixelBufferSize(winSizeH,winSizeV, 10, 10, 10, 10);
    }
    else
    {
        glutInitWindowPosition(0, 0);
        xyWindow = glutCreateWindow("XY");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display3DXY);
        glutKeyboardFunc(keyActions3D);
        glutSpecialFunc(specialInput3D);

        glutInitWindowPosition(winSizeH + 8, 0);
        yzWindow = glutCreateWindow("YZ");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display3DYZ);
        glutKeyboardFunc(keyActions3D);
        glutSpecialFunc(specialInput3D);

        glutInitWindowPosition(winSizeH*2 + 16, 0);
        zxWindow = glutCreateWindow("ZX");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display3DZX);
        glutKeyboardFunc(keyActions3D);
        glutSpecialFunc(specialInput3D);

        CPixelBuffer::instance(xyWindow)->setPixelBufferSize(winSizeH,winSizeV, 0, 0, 0, 0, "xy");
        CPixelBuffer::instance(yzWindow)->setPixelBufferSize(winSizeH,winSizeV, 0, 0, 0, 0, "yz");
        CPixelBuffer::instance(zxWindow)->setPixelBufferSize(winSizeH,winSizeV, 0, 0, 0, 0, "zx");
    }

    CFileIO::loadFile(_3Dmode, curveEditor);

    if(terminalMode)
    {
        //glutIdleFunc(consoleInput);
        glutTimerFunc(100, timer, 0);
    }

	glutMainLoop();
	return 0;
}

void timer( int value )
{
    consoleInput();
    refreshAllWindows();
    glutTimerFunc(100, timer, 0 );
}

void refreshAllWindows()
{
    glutPostWindowRedisplay(xyWindow);
    glutPostWindowRedisplay(yzWindow);
    glutPostWindowRedisplay(zxWindow);
}

void display2D()
{   
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

    CPixelBuffer::instance(0)->resetPixelBuffer(backgroundColor);
    
    if(!curveEditor)
    {
        CShapes::instance()->drawAllShapes2D(drawOutline, useDDA, bFill);
    }
    else
    {
        if(CShapes::instance()->getActiveShape().k > CShapes::instance()->getActiveShape().vertices.size())
        {
            CShapes::instance()->setK(CShapes::instance()->getActiveShape().vertices.size());
        }
        if(CShapes::instance()->getActiveShape().knots.size() < CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k)
        {
            CShapes::instance()->addKnot(0);
        }
        CShapes::instance()->drawAllCurves(curveResolution);
    }

	glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(0)->getPixelBuffer());

	glutSwapBuffers();
}


void mouseDragVertex(int x, int y)
{
    if(modifyingVertex)
    {
        SVertex point;
        point.x = (float)x / (float)winSizeH;
        point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
        CShapes::instance()->modifyVertexActiveShape(point, indexOfVertexToModify);
        glutPostRedisplay();
    }
}

void mouseClick2D(int button, int state, int x, int y)
{
    if(glutGetModifiers() == GLUT_ACTIVE_ALT)
    {
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            float minDist = 100000;
            float distTemp = 0;
            int i = 0;
            int indexToChange = -1;
            for(const auto vertex : CShapes::instance()->getActiveShape().vertices)
            {
                distTemp = point.distance(vertex);
                if(distTemp < minDist)
                {
                    minDist = distTemp;
                    indexToChange = i;
                }
                i++;
            }
            CShapes::instance()->modifyVertexActiveShape(point, indexToChange);
            glutPostRedisplay();
            indexOfVertexToModify = indexToChange;
            modifyingVertex = true;
        }
    }
    else if(glutGetModifiers() == GLUT_ACTIVE_CTRL)
    {
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            float minDist = 100000;
            float distTemp = 0;
            int i = 0;
            int indexToChange = -1;
            for(const auto vertex : CShapes::instance()->getActiveShape().vertices)
            {
                distTemp = point.distance(vertex);
                if(distTemp < minDist)
                {
                    minDist = distTemp;
                    indexToChange = i;
                }
                i++;
            }
            CShapes::instance()->deleteVertexActiveShape(indexToChange);
            glutPostRedisplay();
        }
    }
    else if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && CShapes::instance()->getActiveShape().vertices.size() > 1)
    {
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            float minDist = 100000;
            float distTemp = 0;
            int indexToChange = -1;
            auto test = CShapes::instance()->getActiveShape().vertices;
            for(int i = 0; i < CShapes::instance()->getActiveShape().vertices.size(); i++)
            {
                distTemp = point.distanceToSegment(CShapes::instance()->getActiveShape().vertices[i], CShapes::instance()->getActiveShape().vertices[(i+1) % CShapes::instance()->getActiveShape().vertices.size()]);
                if(distTemp < minDist)
                {
                    minDist = distTemp;
                    indexToChange = i;
                }
            }
            CShapes::instance()->insertVertexActiveShape(point, indexToChange);
            glutPostRedisplay();
        }
    }
    else
    {
        // add a vertex to current shape
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            CShapes::instance()->addVertexToActiveShape(point);
            if(curveEditor)
            {
                if(CShapes::instance()->getActiveShape().vertices.size() < 3)
                {
                    CShapes::instance()->setK(2);
                }
            }
            glutPostRedisplay();
        }
        // done drawing shape
        if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            CShapes::instance()->addActiveToList();
            glutPostRedisplay();
        }
        
        if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            modifyingVertex = false;
            indexOfVertexToModify = -1;
        }
    }
}

void keyActions2D(unsigned char key, int x, int y)
{
    // Toggle line drawing method;
    if(key == 'l')
    {
        useDDA = !useDDA;
        glutPostRedisplay();
    }
    // remove last added point
    else if(key == 'u')
    {
        CShapes::instance()->undoVertexAdd();
        glutPostRedisplay();
    }
    // re-add removed point
    else if(key == 'U')
    {
        CShapes::instance()->redoVertexAdd();
        glutPostRedisplay();
    }
    // toggle filling in shapes or not
    else if(key == 'f')
    {
        bFill = !bFill;
        glutPostRedisplay();
    }
    // toggle drawing outline
    else if(key == 'o')
    {
        drawOutline = !drawOutline;
        glutPostRedisplay();
    }
    // clip
    else if(key == 'c')
    {
        CShapes::instance()->clip2D();
        glutPostRedisplay();
    }
    // translate
    else if(key == 't')
    {
        translate = true;
        borderSizeChange = viewportMove = scale = rotate = false;
    }
    // rotate
    else if(key == 'r')
    {
        rotate = true;
        borderSizeChange = viewportMove = scale = translate = false;
    }
    // scale
    else if(key == 's')
    {
        scale = true;
        borderSizeChange = viewportMove = translate = rotate = false;
    }
    // save to file
    else if(key == 'S')
    {
        CFileIO::saveFile(_3Dmode, curveEditor);
        glutPostRedisplay();
    }
    // move viewport
    else if(key == 'v')
    {
        viewportMove = true;
        borderSizeChange = scale = translate = rotate = false;
    }
    // scale viewport
    else if(key == 'b')
    {
        borderSizeChange = true;
        viewportMove = scale = translate = rotate = false;
    }
    else if(key == '.')
    {
        curveOrder++;
        int max = CShapes::instance()->getActiveShape().vertices.size();
        if(curveOrder > max){curveOrder = max;}
        CShapes::instance()->setK(curveOrder);
        glutPostRedisplay();
    }
    else if(key == ',')
    {
        curveOrder--;
        if(curveOrder < 2){curveOrder = 2;}
        CShapes::instance()->setK(curveOrder);
        glutPostRedisplay();
    }
    else if(key == '/')
    {
        CShapes::instance()->toggleBezier();
        glutPostRedisplay();
    }
    else if(key == ';')
    {
        curveEditor = !curveEditor;
        glutPostRedisplay();
    }
    else if(key == '[')
    {
        if(curveResolution >= 200)
        {
            curveResolution -= 100;
        }
        else if (curveResolution < 200 && curveResolution >= 20)
        {
            curveResolution -= 10;
        }
        else
        {
            curveResolution -= 1;
        }
        if (curveResolution < 2) {curveResolution = 2;}
        glutPostRedisplay();
    }
    else if(key == ']')
    {
        if(curveResolution >= 100)
        {
            curveResolution += 100;
        }
        else if (curveResolution < 100 && curveResolution >= 10)
        {
            curveResolution += 10;
        }
        else
        {
            curveResolution += 1;
        }
        glutPostRedisplay();
    }
    else if(key == 'k' && !CShapes::instance()->getActiveShape().bezier)
    {
        std::cout << "Knots ([knot index]knot value): " << std::endl;
        for(int i=0; i < CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k; i++)
        {
            std::cout << "[" << i+1 << "]" << (float)CShapes::instance()->getActiveShape().knots[i];
            if(i < CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        
        
        std::cout << "Which knot do you want to edit?[1-" << CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k <<"]: " << std::endl;
        std::string knotsIndex;
        int index = 0;
        std::cin >> knotsIndex;
        try
        {
            index = std::stoi(knotsIndex);
        }
        catch (std::invalid_argument){}
        while(index < 1 || index > CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k)
        {
            std::cout << "Invalid index. Please enter a valid index: " << std::endl;
            std::cin >> knotsIndex;
            try
            {
                index = std::stoi(knotsIndex);
            }
            catch (std::invalid_argument){}
        }
        float minKnot, maxKnot;
        if(index == 1)
        {
            minKnot = 0;
        }
        else
        {
            minKnot = CShapes::instance()->getActiveShape().knots[index - 2];
        }
        if(index == CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k)
        {
            maxKnot = std::numeric_limits<float>::infinity();
        }
        else
        {
            maxKnot = CShapes::instance()->getActiveShape().knots[index];
        }
        std::cout << "Enter new value["<< minKnot << "-" << maxKnot <<"]: " << std::endl;
        std::string knotValue;
        float newKnotVal = -1;
        std::cin >> knotValue;
        try
        {
            newKnotVal = std::stof(knotValue);
        }
        catch (std::invalid_argument){}
        while(newKnotVal < minKnot || newKnotVal > maxKnot)
        {
            std::cout << "Invalid knot value. Please enter a valid value: " << std::endl;
            std::cin >> knotValue;
            try
            {
                newKnotVal = std::stof(knotValue);
            }
            catch (std::invalid_argument){}
        }
        CShapes::instance()->modKnot(index-1, newKnotVal);
        glutPostRedisplay();
    }
}

void specialInput2D(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
    {
        if(translate){CShapes::instance()->translate2D('u');}
        else if(viewportMove)
        {
            CPixelBuffer::instance(0)->changeBorder('u', -1);
            CPixelBuffer::instance(0)->changeBorder('d', 1);
        }
        else if(borderSizeChange)
        {
            CPixelBuffer::instance(0)->changeBorder('u', -1);
            CPixelBuffer::instance(0)->changeBorder('d', -1);
        }
    }
    else if(key == GLUT_KEY_DOWN)
    {
        if(translate){CShapes::instance()->translate2D('d');}
        else if(viewportMove)
        {
            CPixelBuffer::instance(0)->changeBorder('u', 1);
            CPixelBuffer::instance(0)->changeBorder('d', -1);
        }
        else if(borderSizeChange)
        {
            CPixelBuffer::instance(0)->changeBorder('u', 1);
            CPixelBuffer::instance(0)->changeBorder('d', 1);
        }
    }
    else if(key == GLUT_KEY_LEFT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getPreviousShape();
        }
        else
        {
            if(translate){CShapes::instance()->translate2D('l');}
            else if(rotate){CShapes::instance()->rotate2D(-1);}
            else if(scale){CShapes::instance()->scale2D(1/1.1);}
            else if(viewportMove)
            {
                CPixelBuffer::instance(0)->changeBorder('l', -1);
                CPixelBuffer::instance(0)->changeBorder('r', 1);
            }
            else if(borderSizeChange)
            {
                CPixelBuffer::instance(0)->changeBorder('l', 1);
                CPixelBuffer::instance(0)->changeBorder('r', 1);
            }
        }
    }
    else if(key == GLUT_KEY_RIGHT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getNextShape();
        }
        else
        {
            if(translate){CShapes::instance()->translate2D( 'r');}
            else if(rotate){CShapes::instance()->rotate2D( 1);}
            else if(scale){CShapes::instance()->scale2D( 1.1);}
            else if(viewportMove)
            {
                CPixelBuffer::instance(0)->changeBorder('l', 1);
                CPixelBuffer::instance(0)->changeBorder('r', -1);
            }
            else if(borderSizeChange)
            {
                CPixelBuffer::instance(0)->changeBorder('l', -1);
                CPixelBuffer::instance(0)->changeBorder('r', -1);
            }
        }
    }
    glutPostRedisplay();
}

void display3DXY()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    CPixelBuffer::instance(xyWindow)->resetPixelBuffer(backgroundColor);

    
    CShapes::instance()->drawAllShapes3DXY(xyWindow, wire, viewPointXY);

    if(rotationLineInput)
    {
        float zoomMult = CPixelBuffer::instance(xyWindow)->getZoom();
        SVertex startv;
        startv.x = rotStartPt.x*zoomMult;
        startv.y = rotStartPt.y*zoomMult;
        SVertex endv;
        endv.x = rotEndPt.x*zoomMult;
        endv.y = rotEndPt.y*zoomMult;
        CGraphLib::drawLineB2D(startv, endv,  CColor::customColor(1,0,0), xyWindow);
    }

    glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(xyWindow)->getPixelBuffer());
    glutSwapBuffers();
}

void display3DYZ()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    CPixelBuffer::instance(yzWindow)->resetPixelBuffer(backgroundColor);

    
    CShapes::instance()->drawAllShapes3DYZ(yzWindow, wire, viewPointYZ);

    if(rotationLineInput)
    {
        float zoomMult = CPixelBuffer::instance(yzWindow)->getZoom();
        SVertex startv;
        startv.x = rotStartPt.x*zoomMult;
        startv.y = rotStartPt.y*zoomMult;
        SVertex endv;
        endv.x = rotEndPt.x*zoomMult;
        endv.y = rotEndPt.y*zoomMult;
        CGraphLib::drawLineB2D(startv, endv, CColor::customColor(1,0,0), yzWindow);
    }

    glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(yzWindow)->getPixelBuffer());
    glutSwapBuffers();
}

void display3DZX()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    CPixelBuffer::instance(zxWindow)->resetPixelBuffer(backgroundColor);

    
    CShapes::instance()->drawAllShapes3DZX(zxWindow, wire, viewPointZX);

    if(rotationLineInput)
    {
        float zoomMult = CPixelBuffer::instance(zxWindow)->getZoom();
        SVertex startv;
        startv.x = rotStartPt.x*zoomMult;
        startv.y = rotStartPt.y*zoomMult;
        SVertex endv;
        endv.x = rotEndPt.x*zoomMult;
        endv.y = rotEndPt.y*zoomMult;
        CGraphLib::drawLineB2D(startv, endv, CColor::customColor(1,0,0), zxWindow);
    }

    glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(zxWindow)->getPixelBuffer());
    glutSwapBuffers();
}

void consoleInput()
{
    if(!rotationLineInput)
    {
        int count = 0;
        std::string userInput = "";
        std::string token = "";
        std::cout << "\nCommands: n - next 3D shape, p - prev 3D shape, t - translate, r - rotate, s - scale, z - zoom to fit, half-tone - toggle half tone mode, wire - toggle wireframe mode, light - move light source, iA - change ambient light intensity, iL - change light source intensity, K - change K, phong - change Phong constant, kA - change Ambient constant, kD - change diffuse constant, kS - change specular constant, q - quit." << std::endl;
        std::cout << "Enter command:" << std::endl;
        std::cin >> userInput;
        if(userInput == "s")
        {
            std::cout << "Enter scale factor:" ;
            std::cin >> userInput;
            CShapes::instance()->scale3D(stof(userInput), stof(userInput), stof(userInput));
        }
        else if (userInput == "t")
        {
            float x = 0, y = 0, z = 0;
            std::cout << "Enter X value:" ;
            std::cin >> userInput;
            x = stof(userInput);
            std::cout << "Enter Y value:" ;
            std::cin >> userInput;
            y = stof(userInput);
            std::cout << "Enter Z value:" ;
            std::cin >> userInput;
            z = stof(userInput);
            CShapes::instance()->translate3D(x, y, z);
        }
        else if (userInput == "r")
        {
            float x = 0, y = 0, z = 0;
            float x2 = 0, y2 = 0, z2 = 0;
            float degree = 0;
            std::cout << "Enter X value for p1:" ;
            std::cin >> userInput;
            x = stof(userInput);
            std::cout << "Enter Y value for p1:" ;
            std::cin >> userInput;
            y = stof(userInput);
            std::cout << "Enter Z value for p1:" ;
            std::cin >> userInput;
            z = stof(userInput);
            std::cout << "Enter X value for p2:" ;
            std::cin >> userInput;
            x2 = stof(userInput);
            std::cout << "Enter Y value for p2:" ;
            std::cin >> userInput;
            y2 = stof(userInput);
            std::cout << "Enter Z value for p2:" ;
            std::cin >> userInput;
            z2 = stof(userInput);
            SVertex start;
            start.x = x;
            start.y = y;
            start.z = z;
            SVertex end;
            end.x = x2;
            end.y = y2;
            end.z = z2;
            std::cout << "Enter degrees:" ;
            std::cin >> userInput;
            degree = stof(userInput);
            rotationLineInput = true;
            rotStartPt = start;
            rotEndPt = end;
            rotDegree = degree;
        }
        else if (userInput == "n")
        {
            CShapes::instance()->getNextShape();
        }
        else if (userInput == "p")
        {
            CShapes::instance()->getPreviousShape();
        }
        else if (userInput == "z")
        {
            CPixelBuffer::instance(xyWindow)->changeZoom();
            CPixelBuffer::instance(yzWindow)->changeZoom();
            CPixelBuffer::instance(zxWindow)->changeZoom();
        }
        else if (userInput == "half-tone")
        {
            CGraphLib::toggleHalfTone();
        }
        else if(userInput == "wire")
        {
            wire = !wire;
        }
        else if(userInput == "light")
        {
            float x,y,z;
            std::cout << "Enter X value for light:" ;
            std::cin >> userInput;
            x = stof(userInput);
            std::cout << "Enter Y value for light:" ;
            std::cin >> userInput;
            y = stof(userInput);
            std::cout << "Enter Z value for light:" ;
            std::cin >> userInput;
            z = stof(userInput);
            CGraphLib::setLight(x, y, z);
        }
        else if(userInput == "iA")
        {
            float ia;
            std::cout << "Enter value for ambient light intensity:" ;
            std::cin >> userInput;
            ia = stof(userInput);
            CGraphLib::setAmbientLightIntensity(ia);
        }
        else if(userInput == "iL")
        {
            float il;
            std::cout << "Enter value for light source intensity:" ;
            std::cin >> userInput;
            il = stof(userInput);
            CGraphLib::setLightSourceIntensity(il);
        }
        else if(userInput == "K")
        {
            float k;
            std::cout << "Enter value for K:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setK(k);
        }
        else if(userInput == "phong")
        {
            float k;
            std::cout << "Enter value for Phong constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setPhongConstant(k);
        }
        else if(userInput == "kA")
        {
            float k;
            std::cout << "Enter value for Ambient constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setAmbientLightConstant(k);
        }
        else if(userInput == "kD")
        {
            float k;
            std::cout << "Enter value for Diffuse constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setDiffuseConstant(k);
        }
        else if(userInput == "kS")
        {
            float k;
            std::cout << "Enter value for Specular constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setSpecularConstant(k);
        }
        else if(userInput == "q")
        {
            exit(0);
        }
    }
    else
    {
        std::string userInput = "";
        std::cout << "Proceed with rotation? y/n:" ;
        std::cin >> userInput;
        if(userInput == "y" || userInput == "Y")
        {
            CShapes::instance()->rotate3D(rotStartPt, rotEndPt, rotDegree);
        }
        rotationLineInput = false;
    }
    refreshAllWindows();
    return;
}

void keyActions3D(unsigned char key, int x, int y)
{
    // translate
    if(key == 't')
    {
        translate = true;
        scale = rotate = lightSource = false;
    }
    // rotate
    else if(key == 'r')
    {
        rotate = true;
        scale = translate = lightSource = false;
    }
    // scale
    else if(key == 's')
    {
        scale = true;
        translate = rotate = lightSource = false;
    }
    else if(key == 'S')
    {
        CFileIO::saveFile(_3Dmode, curveEditor);
        refreshAllWindows();
    }
    else if(key == 'z')
    {
        CPixelBuffer::instance(xyWindow)->changeZoom();
        CPixelBuffer::instance(yzWindow)->changeZoom();
        CPixelBuffer::instance(zxWindow)->changeZoom();
        refreshAllWindows();
    }
    else if(key == 'h')
    {
        CGraphLib::toggleHalfTone();
        refreshAllWindows();
    }
    else if (key == 'l')
    {
        lightSource = true;
        scale = translate = rotate = false;
    }
    else if(key == 'w')
    {
        wire = !wire;
        refreshAllWindows();
    }
}


void specialInput3D(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
    {
        if(translate)
        {
            if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->translate3D( 0.0, 0.05, 0.0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->translate3D( 0, 0, 0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->translate3D( 0.05, 0, 0);
            }
        }
        else if(rotate)
        {
            SVertex pt1 = CShapes::instance()->findCentroid3D();
            SVertex pt2 = pt1;
            if(glutGetWindow() == xyWindow)
            {
                pt2.x += 0.5;
                CShapes::instance()->rotate3D( pt1, pt2, 5);
            }
            else if (glutGetWindow() == yzWindow)
            {
                pt2.y += 0.5;
                CShapes::instance()->rotate3D( pt1, pt2, 5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                pt2.z += 0.5;
                CShapes::instance()->rotate3D( pt1, pt2, 5);
            }
        }
        else if(scale)
        {
            if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
            {
                CShapes::instance()->scale3D(1.5, 1.5, 1.5);
            }
            else if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->scale3D(1, 1.5, 1);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->scale3D( 1, 1, 1.5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->scale3D( 1.5, 1, 1);
            }
        }
        else if(lightSource)
        {
            if(glutGetWindow() == xyWindow)
            {
                CGraphLib::moveLight(0, 0.05, 0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CGraphLib::moveLight(0, 0, 0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CGraphLib::moveLight(0.05, 0, 0);
            }
        }
    }
    else if(key == GLUT_KEY_DOWN)
    {
        if(translate)
        {
            if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->translate3D( 0.0, -0.05, 0.0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->translate3D( 0, 0, -0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->translate3D( -0.05, 0, 0);
            }
        }
        else if(rotate)
        {
            SVertex pt1 = CShapes::instance()->findCentroid3D();
            SVertex pt2 = pt1;
            if(glutGetWindow() == xyWindow)
            {
                pt2.x -= 0.5;
                CShapes::instance()->rotate3D(pt1, pt2, 5);
            }
            else if (glutGetWindow() == yzWindow)
            {
                pt2.y -= 0.5;
                CShapes::instance()->rotate3D(pt1, pt2, 5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                pt2.z -= 0.5;
                CShapes::instance()->rotate3D(pt1, pt2, 5);
            }
        }
        else if(scale)
        {
            if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
            {
                CShapes::instance()->scale3D( 1/1.5, 1/1.5, 1/1.5);
            }
            else if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->scale3D( 1, 1/1.5, 1);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->scale3D( 1, 1, 1/1.5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->scale3D( 1/1.5, 1, 1);
            }
        }
        else if(lightSource)
        {
            if(glutGetWindow() == xyWindow)
            {
                CGraphLib::moveLight(0, -0.05, 0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CGraphLib::moveLight(0, 0, -0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CGraphLib::moveLight(-0.05, 0, 0);
            }
        }
    }
    else if(key == GLUT_KEY_LEFT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getPreviousShape();
        }
        else
        {
            if(translate)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->translate3D( -0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->translate3D(0.0, -0.05, 0.0);
                    
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->translate3D(0, 0, -0.05);
                }
            }
            else if(rotate)
            {
                SVertex pt1 = CShapes::instance()->findCentroid3D();
                SVertex pt2 = pt1;
                if(glutGetWindow() == xyWindow)
                {
                    pt2.y -= 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    pt2.z -= 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    pt2.x -= 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
            }
            else if(scale)
            {
                if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
                {
                    CShapes::instance()->scale3D(1/1.5, 1/1.5, 1/1.5);
                }
                else if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->scale3D(1/1.5, 1, 1);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->scale3D(1, 1/1.5, 1);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->scale3D(1, 1, 1/1.5);
                }
            }
            else if(lightSource)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CGraphLib::moveLight(-0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CGraphLib::moveLight(0, -0.05, 0);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CGraphLib::moveLight(0, 0, -0.05);
                }
            }
        }
    }
    else if(key == GLUT_KEY_RIGHT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getNextShape();
        }
        else
        {
            if(translate)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->translate3D(0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->translate3D(0.0, 0.05, 0.0);
                    
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->translate3D(0, 0, 0.05);
                }
            }
            else if(rotate)
            {
                SVertex pt1 = CShapes::instance()->findCentroid3D();
                SVertex pt2 = pt1;
                if(glutGetWindow() == xyWindow)
                {
                    pt2.y += 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    pt2.z += 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    pt2.x += 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
            }
            else if(scale)
            {
                if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
                {
                    CShapes::instance()->scale3D( 1.5, 1.5, 1.5);
                }
                else if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->scale3D( 1.5, 1, 1);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->scale3D( 1, 1.5, 1);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->scale3D( 1, 1, 1.5);
                }
            }
            else if(lightSource)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CGraphLib::moveLight(0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CGraphLib::moveLight(0, 0.05, 0);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CGraphLib::moveLight(0, 0, 0.05);
                }
            }
        }
    }
    refreshAllWindows();
}