#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

#include <FL/Fl.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include "OptNet.h"
#include "OptNetDisplay.h"
#include "SRGGraph.h"
#include "SRGDisplay.h"

class GraphWindow : public Fl_Gl_Window {
    SRGDisplay * disp;
    OptNetDisplay * disp2;
    GLfloat * vertices, *keyVertices;
    GLfloat * colours;
    GLuint *edges, *nodes, *opedges;
    GLuint m,n,opm;
    void draw();
    void resize(int X,int Y,int W,int H);
public:
    GraphWindow(SRGGraph& g, OptNet& o, int X,int Y,int W,int H,const char*L=0);
    ~GraphWindow();
};

#endif // GRAPHWINDOW_H
