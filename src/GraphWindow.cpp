#include "..\include\GraphWindow.h"
#include <GL/glu.h>

void GraphWindow::resize(int X,int Y,int W,int H)
{
        Fl_Gl_Window::resize(X,Y,W,H);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glOrtho(-w(),w(),-h(),h(),-1,1);
        glMatrixMode(GL_MODELVIEW);
        glScalef(0.8,0.8,0.8);

        glViewport(0,0,w(),h());
        redraw();
}

void GraphWindow::draw()
{

    if (!valid()) {
        valid(1);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glOrtho(-w(),w(),-h(),h(),-1,1);
        glMatrixMode(GL_MODELVIEW);
        glScalef(0.8,0.8,0.8);

        glViewport(0,0,w(),h());
        edges = new GLuint [m*2];
        disp->getIndices(edges);

        nodes = new GLuint[n];
        for(int i=0; i<n; ++i)
            nodes[i]=i;

        glEnable(GL_POINT_SMOOTH);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glEnable (GL_BLEND);
        glEnable (GL_FOG);
        glEnable(GL_MULTISAMPLE);
        glShadeModel (GL_FLAT);
        glPointSize(10.0);
        glHint(GL_POINT_SMOOTH, GL_NICEST);
    }
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw white 'X'
    glColor4f(0.4, 0.4, 1, 0.9);

    vertices = disp->getCoords();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,vertices);

    glDrawElements(GL_LINES, 2*m, GL_UNSIGNED_INT, edges);

    glColor4f(1, 1, 1,0.95);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, nodes);
    /*glPointSize(9.0);
    glColor4f(0.3, 1, 0.3,0.333);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, indices);
    glPointSize(10.0);
    glColor4f(0.3, 1, 0.3,0.333);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, indices);*/


    glDisable(GL_VERTEX_ARRAY);
}

GraphWindow::GraphWindow(SRGGraph& g, int X,int Y,int W,int H,const char*L) : Fl_Gl_Window(X,Y,W,H,L)
{
        disp = new SRGDisplay(W,H,g);
        n = g.size();
        m = g.edgeCount();
}//

GraphWindow::~GraphWindow()
{

    if(vertices != NULL)
        delete [] vertices;
    if(edges != NULL)
        delete [] edges;
    if(nodes != NULL)
        delete [] edges;
    if(disp != NULL)
        delete disp;
}
