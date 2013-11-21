#ifndef OPTNETDISPLAY_H
#define OPTNETDISPLAY_H

#include "OptNet.h"
#include <GL/gl.h>

class OptNetDisplay
{
    OptNet& graph;
    GLuint size;
public:
    OptNetDisplay(int w, int h, OptNet& g);
    void getIndices(GLuint*);
};

#endif // OPTNETDISPLAY_H
