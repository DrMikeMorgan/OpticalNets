#include "..\include\GraphWindow.h"
#include <GL/glu.h>
#include <cmath>

void GraphWindow::resize(int X,int Y,int W,int H)
{
        Fl_Gl_Window::resize(X,Y,W,H);
        disp->resize(w(),h());
        glLoadIdentity();
        //glMatrixMode(GL_PROJECTION);
        glOrtho(-w(),w(),-h(),h(),-1,1);
        //glMatrixMode(GL_MODELVIEW);
        glScalef(0.96,0.96,0.96); //keep stuff from going over the edge

        redraw();
}

void GraphWindow::draw()
{

    if (!valid()) {
        valid(1);
        glLoadIdentity();
        //glMatrixMode(GL_PROJECTION);
        glOrtho(-w(),w(),-h(),h(),-1,1);
        //glMatrixMode(GL_MODELVIEW);
        glScalef(0.96,0.96,0.96); //keep stuff from going over the edge


        edges = new GLuint [m*2];
        disp->getIndices(edges);
        opedges = new GLuint [opm * 2];
        disp2->getIndices(opedges);
        colours = disp->getColours();

        nodes = new GLuint[n];
        for(int i=0; i<n; ++i)
            nodes[i]=i;

        keyVertices = new GLfloat[n*2];
        for(int i=0; i<n; ++i)
        {
            keyVertices[i*2] = w()-5;
            keyVertices[i*2+1] = h() - i*((h()-20)/n) - 10;
        }

        glEnable(GL_POINT_SMOOTH);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glEnable (GL_BLEND);
        glEnable(GL_MULTISAMPLE);
        glShadeModel (GL_SMOOTH);
        glPointSize(10.0);
        glHint(GL_POINT_SMOOTH, GL_NICEST);
    }
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw white 'X'

    glViewport(0,0,w()/2,h()); //draw SRG half
    vertices = disp->getCoords();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,vertices);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4,GL_FLOAT,0,colours);
    glDrawElements(GL_LINES, 2*m, GL_UNSIGNED_INT, edges);
    glPointSize(sqrt(h()*w())/50.0);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, nodes);


    glViewport(w()/2,0,w()/2,h());
    glVertexPointer(2,GL_FLOAT,0,vertices);
    glDrawElements(GL_LINES, 2*opm, GL_UNSIGNED_INT, opedges);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, nodes);
    glVertexPointer(2,GL_FLOAT,0,keyVertices);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, nodes);


    glDisable(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_ARRAY);
}

GraphWindow::GraphWindow(SRGGraph& g, OptNet& o, int X,int Y,int W,int H,const char*L) : Fl_Gl_Window(X,Y,W,H,L)
{
        disp = new SRGDisplay(W,H,g);
        disp2 = new OptNetDisplay(W,H,o);
        n = g.size();
        m = g.edgeCount();
        opm = o.edgeCount();
}//

GraphWindow::~GraphWindow()
{

    if(vertices != NULL)
        delete [] vertices;
    if(edges != NULL)
        delete [] edges;
    if(nodes != NULL)
        delete [] nodes;
    if(colours != NULL)
        delete [] colours;
    if(keyVertices != NULL)
        delete [] keyVertices;
    if(disp != NULL)
        delete disp;
}
