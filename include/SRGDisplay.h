#ifndef SRGDISPLAY_H
#define SRGDISPLAY_H
#include <GL/gl.h>
#include "SRGGraph.h"


class SRGDisplay
{
    SRGGraph& graph;
    GLfloat * coords;
    GLuint size;
    int wi, hi;
public:
    SRGDisplay(int w, int h, SRGGraph & g);
    GLfloat * getCoords(){return coords;}
    void getIndices(GLuint*);
    void resize(int w, int h);
    //one for colours?
};



#endif // SRGDISPLAY_H
