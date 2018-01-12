#include <string>

#ifndef FILEIO_H
#define FILEIO_H

class CFileIO
{
public:
    static void loadFile(bool  _3Dmode, bool curveMode);
    static void saveFile(bool _3Dmode, bool curveMode);
};

#endif 