#ifndef SRGDISPLAY_H
#define SRGDISPLAY_H
#include <GL/gl.h>
#include "SRGGraph.h"

class SRGDisplay
{
    SRGGraph& graph;
    GLfloat * coords;
    GLfloat * colours;

    GLuint size;
    int wi, hi;
public:
    SRGDisplay(int w, int h, SRGGraph & g);
    GLfloat * getCoords(){return coords;}
    void getIndices(GLuint*);
    GLfloat * getColours(){return colours;}
    void resize(int w, int h);
};



#endif // SRGDISPLAY_H
