#include "FileIO.h"
#include "Shapes.h"
#include <iostream>
#include <fstream>
#include <ctime>

// Function to load the file needed for the selected mode.
void CFileIO::loadFile(bool _3Dmode, bool curveMode)
{
    if(!_3Dmode)
    {
        if(!curveMode)
        {
            std::ifstream inputFile("2DData.txt");
            if(inputFile.is_open())
            {
                SVertex inputVertex;
                std::string line;
                if(getline (inputFile,line))
                {
                    int numShapes = std::stoi(line);
                    int numVertices = 0;
                    std::cout << line << std::endl << std::endl;
                    for(int j = 0; j < numShapes; j++)
                    {
                        inputFile >> line;
                        numVertices = std::stoi(line);
                        std::cout << line << std::endl;
                        for(int i = 0; i < numVertices; i++)
                        {
                            inputFile >> line;
                            std::cout << line << " ";
                            inputVertex.x = std::stof(line);
                            inputFile >> line;
                            std::cout << line << std::endl;
                            inputVertex.y = std::stof(line);
                            CShapes::instance()->addVertexToActiveShape(inputVertex);
                        }
                        std::cout << std::endl;
                        if(j < numShapes - 1)
                        {
                        CShapes::instance()->addActiveToList();
                        }
                    }
                    std::cout << "File Loaded!" << std::endl;
                }
                inputFile.close();
            }
        }
        else
        {
            std::ifstream inputFile("curveData.txt");
            if(inputFile.is_open())
            {
                SVertex inputVertex;
                std::string line;
                if(getline (inputFile,line))
                {
                    int numShapes = std::stoi(line);
                    int numVertices = 0;
                    int k = 0;
                    std::cout << line << std::endl << std::endl;
                    for(int j = 0; j < numShapes; j++)
                    {
                        inputFile >> line;
                        numVertices = std::stoi(line);
                        std::cout << line << std::endl;
                        for(int i = 0; i < numVertices; i++)
                        {
                            inputFile >> line;
                            std::cout << line << " ";
                            inputVertex.x = std::stof(line);
                            inputFile >> line;
                            std::cout << line << std::endl;
                            inputVertex.y = std::stof(line);
                            CShapes::instance()->addVertexToActiveShape(inputVertex);
                        }
                        inputFile >> line;
                        k = std::stoi(line);
                        std::cout << line << std::endl;
                        if(k > 0)
                        {
                            CShapes::instance()->setK(k);
                            CShapes::instance()->addKnot(0);
                            CShapes::instance()->setBezier(false);
                            for(int m = 0; m < k + numVertices; m++)
                            {
                                inputFile >> line;
                                std::cout << line << " ";
                                CShapes::instance()->modKnot(m, std::stof(line));
                            }
                            std::cout << std::endl;
                        }
                        else
                        {
                            CShapes::instance()->setBezier(true);
                        }
                        std::cout << std::endl;
                        if(j < numShapes - 1)
                        {
                        CShapes::instance()->addActiveToList();
                        }
                    }
                    std::cout << "File Loaded!" << std::endl;
                }
                inputFile.close();
            }
        }
    }
    else
    {
        std::ifstream inputFile("3DData.txt");
        if(inputFile.is_open())
        {
            SVertex inputVertex;
            SEdge inputEdge;
            STriangle inputTriangle;
            std::string line;
            if(getline (inputFile,line))
            {
                int numShapes = std::stoi(line);
                int numVertices = 0;
                int numEdges = 0;
                int numTriangles = 0;
                std::cout << line << std::endl << std::endl;
                for(int j = 0; j < numShapes; j++)
                {
                    srand(time(NULL) * rand() * time(NULL));
                    float r = ((rand() * time(NULL) )% 1000)/1000.f;
                    srand(time(NULL) * rand() * time(NULL));
                    float g = ((rand() * time(NULL)) % 1000)/1000.f;
                    srand(time(NULL) * rand() * time(NULL));
                    float b = ((rand() * time(NULL)) % 1000)/1000.f;
                    CColor color = CColor(r,g,b);
                    inputFile >> line;
                    numVertices = std::stoi(line);
                    std::cout << line << std::endl;
                    for(int i = 0; i < numVertices; i++)
                    {
                        inputFile >> line;
                        std::cout << line << " ";
                        inputVertex.x = (std::stof(line));
                        inputFile >> line;
                        std::cout << line << " ";
                        inputVertex.y = (std::stof(line));
                        inputFile >> line;
                        std::cout << line << std::endl;
                        inputVertex.z = (std::stof(line));
                        CShapes::instance()->addVertexToActiveShape(inputVertex);
                    }
                    inputFile >> line;
                    numEdges = std::stoi(line);
                    std::cout << line << std::endl;
                    for(int i = 0; i < numEdges; i++)
                    {
                        inputFile >> line;
                        std::cout << line << " ";
                        inputEdge.p1 = (std::stof(line));
                        inputFile >> line;
                        std::cout << line << std::endl;
                        inputEdge.p2 = (std::stof(line));
                        CShapes::instance()->addEdgeToActiveShape(inputEdge);
                    }
                    inputFile >> line;
                    numTriangles = std::stoi(line);
                    std::cout << line << std::endl;
                    for(int i = 0; i < numTriangles; i++)
                    {
                        inputFile >> line;
                        std::cout << line << " ";
                        inputTriangle.vertexIndices[0] = std::stof(line)-1;
                        inputTriangle.vertices[0] = (CShapes::instance()->getActiveShape().vertices[inputTriangle.vertexIndices[0]]);
                        inputFile >> line;
                        std::cout << line << " ";
                        inputTriangle.vertexIndices[1] = std::stof(line)-1;
                        inputTriangle.vertices[1] = (CShapes::instance()->getActiveShape().vertices[inputTriangle.vertexIndices[1]]);
                        inputFile >> line;
                        std::cout << line << std::endl;
                        inputTriangle.vertexIndices[2] = std::stof(line)-1;
                        inputTriangle.vertices[2] = (CShapes::instance()->getActiveShape().vertices[inputTriangle.vertexIndices[2]]);
                        inputTriangle.defaultColor = color;
                        CShapes::instance()->addTriangleToActiveShape(inputTriangle);
                    }
                    std::cout << std::endl;
                    if(j < numShapes - 1)
                    {
                       CShapes::instance()->addActiveToList();
                    }
                }
                std::cout << "File Loaded!" << std::endl;
            }
            inputFile.close();
        }
    }
}

void CFileIO::saveFile(bool _3Dmode, bool curveMode)
{
    CShapes::instance()->addActiveToList();
    
    if(!_3Dmode)
    {
        if(!curveMode)
        {
            std::ofstream outputFile ("2DData.txt");
            if(outputFile.is_open())
            {
                outputFile << CShapes::instance()->getAllShapes().size() <<  "\n";
                for(SShape  tShape : CShapes::instance()->getAllShapes())
                {
                    outputFile <<  "\n" << tShape.vertices.size() << "\n";
                    for(int i = 0; i < tShape.vertices.size(); i++)
                    {
                        outputFile.precision(3);
                        outputFile << std::fixed << tShape.vertices[i].x << "\t" << tShape.vertices[i].y << "\n";
                    }
                }
                outputFile.close();
                std::cout << "File Saved!" << std::endl;
            } 
            else
            {
                std::cout << "File Save ERROR!" << std::endl;
            }
        }
        else
        {
            std::ofstream outputFile ("curveData.txt");
            if(outputFile.is_open())
            {
                outputFile << CShapes::instance()->getAllShapes().size() <<  "\n";
                for(SShape  tShape : CShapes::instance()->getAllShapes())
                {
                    outputFile <<  "\n" << tShape.vertices.size() << "\n";
                    for(int i = 0; i < tShape.vertices.size(); i++)
                    {
                        outputFile.precision(3);
                        outputFile << std::fixed << tShape.vertices[i].x << "\t" << tShape.vertices[i].y << "\n";
                    }
                    if(!tShape.bezier)
                    {
                        outputFile << tShape.k << "\n";
                        for(int i = 0; i < tShape.vertices.size() + tShape.k; i++)
                        {
                            outputFile << tShape.knots[i] << "\t";
                        }
                        outputFile << "\n";
                    }
                    else
                    {
                        outputFile << 0 << "\n";
                    }
                }
                outputFile.close();
                std::cout << "File Saved!" << std::endl;
            }
            else
            {
                std::cout << "File Save ERROR!" << std::endl;
            }
        }
    }
    else
    {
        std::ofstream outputFile ("3DData.txt");
        if(outputFile.is_open())
        {
            outputFile << CShapes::instance()->getAllShapes().size() <<  "\n";
            for(SShape tShape : CShapes::instance()->getAllShapes())
            {
                outputFile <<  "\n" << tShape.vertices.size() << "\n";
                for(int i = 0; i < tShape.vertices.size(); i++)
                {
                    outputFile.precision(3);
                    outputFile << std::fixed << tShape.vertices[i].x << "\t" << tShape.vertices[i].y << "\t" << tShape.vertices[i].z << "\n";
                }
                outputFile << tShape.edges.size() << "\n";
                for(int i = 0; i < tShape.edges.size(); i++)
                {
                    outputFile.precision(0);
                    outputFile << std::fixed << tShape.edges[i].p1 << "\t" << tShape.edges[i].p2 << "\n";
                }
                outputFile << tShape.triangles.size() << "\n";
                for(int i = 0; i < tShape.triangles.size(); i++)
                {
                    outputFile.precision(0);
                    outputFile << std::fixed << tShape.triangles[i].vertexIndices[0] << "\t" << tShape.triangles[i].vertexIndices[1] << "\t" << tShape.triangles[i].vertexIndices[2]<< "\n";
                }
            }
            outputFile.close();
            std::cout << "File Saved!" << std::endl;
        }
        else
        {
            std::cout << "File Save ERROR!" << std::endl;
        }
    }
}