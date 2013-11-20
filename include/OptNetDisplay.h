#ifndef OPTNETDISPLAY_H
#define OPTNETDISPLAY_H

#include "OptNet.h"
#include <GL/gl.h>

class OptNetDisplay
{
    OptNet& graph;
    GLfloat * coords;
    GLuint size;
    int wi, hi;
public:
    OptNetDisplay(int w, int h, OptNet & g);
    GLfloat * getCoords(){return coords;}
    void getIndices(GLuint*);
    void resize(int w, int h);
    //one for colours?
};

#endif // OPTNETDISPLAY_H
