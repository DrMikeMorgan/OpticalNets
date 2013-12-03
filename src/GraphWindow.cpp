#include "../include/GraphWindow.h"
#include <GL/glext.h>
#include <GL/glu.h>
#include <cmath>

void GraphWindow::resize(int X,int Y,int W,int H)
{
        Fl_Gl_Window::resize(X,Y,W,H);
        for(int i=0; i<n; ++i)
        {
            vertices[i*2] = (GLfloat) (o->getCoords())[i*2] * w();
            vertices[i*2+1] = (GLfloat) (o->getCoords())[i*2+1] * h();
        }
        for(int i=0; i<n; ++i)
        {
            keyVertices[i*2] = w()-5;
            keyVertices[i*2+1] = h() - i*((h()-20)/n) - 10;
        }

        glLoadIdentity();
        glOrtho(0,w(),0,h(),-1,1);
        redraw();
}

void GraphWindow::draw()
{
    if (!valid()) {
        valid(1);
        glLoadIdentity();
        glOrtho(0,w(),0,h(),-1,1);

        glEnable(GL_POINT_SMOOTH);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glEnable (GL_BLEND);
        glShadeModel (GL_SMOOTH);
        glPointSize(10.0);
    }

    if(n==0)
        return;

    if(renderSRGs) //maybe draw after optnet?
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0,0,w()/2,h()); //draw SRG half
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2,GL_FLOAT,0,vertices);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4,GL_FLOAT,0,colours);
        glDrawElements(GL_LINES, 2*m, GL_UNSIGNED_INT, edges);
        glPointSize(sqrt(h()*w())/50.0);
        glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, nodes);
    }
    else
        glClear(GL_COLOR_BUFFER_BIT);

    glViewport(w()/2,0,w()/2,h());
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,vertices);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4,GL_FLOAT,0,colours);
    glDrawElements(GL_LINES, 2*opm, GL_UNSIGNED_INT, opedges);
    glPointSize(sqrt(h()*w())/50.0);
    glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, nodes);
    //glVertexPointer(2,GL_FLOAT,0,keyVertices);
    //glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, keyVertices);

    glDisable(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_ARRAY);
}

int GraphWindow::handle(int event)
{
    switch(event)
    {
        case FL_PUSH:
            for(int i=0; i<n; ++i)
            {
                int x2 = ( (Fl::event_x()%(w()/2))*2 - vertices[i*2]) * ( (Fl::event_x()%(w()/2))*2 - vertices[i*2]);
                int y2 = ( (h()-Fl::event_y()) - vertices[i*2+1]) * ( (h()-Fl::event_y()) - vertices[i*2+1]);
                if(sqrt(x2 + y2) <  sqrt(h()*w())/50.0 )
                    selectedNode = i;
            }
            redraw();
            return 1;
        case FL_DRAG:
            if(selectedNode != n)
            {
                vertices[selectedNode*2] = (GLfloat) ( Fl::event_x()%(w()/2) * 2 ) ;
                vertices[selectedNode*2+1] = h() - (GLfloat) Fl::event_y() ;
                redraw();
            }
            return 1;
        case FL_RELEASE:
            selectedNode = n; return 1;
        default:
            return Fl_Gl_Window::handle(event);
    }
    return 0;
}

GraphWindow::GraphWindow(int X,int Y,int W,int H,const char*L) : Fl_Gl_Window(X,Y,W,H,L)
{

    n = 0;
    m = 0;
    selectedNode = 0;
    opm = 0;
    opedges = 0;
    nodes = 0;
    keyVertices = 0;
    colours = 0;
    vertices = 0;
    edges = 0;
    renderSRGs = false;
}

void GraphWindow::SetGraph(SRGGraph& gph)
{
    this->g = &gph;
    m = g->edgeCount();

    if(edges)
        delete [] edges;
    edges = new GLuint[m*2];

    int k=0;
    for(GLuint i=0; i<n; ++i)
        for(std::list<SRGGraph::Edge>::iterator j = gph[i].edges.begin(); j != gph[i].edges.end(); ++j)
        {
            if(i < j->dest)
            {
                edges[k++] = i;
                edges[k++] = (GLuint) j->dest;
            }
        }
}

void GraphWindow::SetGraph(OptNet& op)
{
    o = &op;
    n = op.size();
    selectedNode = n;
    opm = op.edgeCount();

    if(nodes)
        delete [] nodes;

    nodes = new GLuint[n];
        for(int i=0; i<n; ++i)
            nodes[i]=i;

    if(vertices)
        delete [] vertices;
    vertices = new GLfloat[op.size()*2];

    if(keyVertices)
        delete [] keyVertices;
    keyVertices = new GLfloat[n*2];

    for(int i=0; i<n; ++i)
    {
        keyVertices[i*2] = w()-5;
        keyVertices[i*2+1] = h() - i*((h()-20)/n) - 10;
    }

    if(colours)
        delete [] colours;
    colours = new GLfloat[op.size()*4];

    for(int i=0; i<n; ++i)
    {
        vertices[i*2] = (GLfloat) (o->getCoords())[i*2] * w();
        vertices[i*2+1] = (GLfloat) (o->getCoords())[i*2+1] * h();
    }

    float offsets [] = {0,1,2};
    float baseCol [] = {0.8,1.0,1.0};
    float baseWeight = 0.7;

    for(int i=0; i<n; ++i)
    {
        for(int j=0; j<3; ++j)
            colours[i*4+j] = ( (baseWeight*baseCol[j]) + ( (2-baseWeight) * GLfloat(rand())/RAND_MAX) )/2;
        colours[i*4+3] = 0.9;
    }


    if(opedges)
        delete [] opedges;
    opedges = new GLuint [opm * 2];

    int k=0;
    for(std::size_t i=0; i<n; ++i)
        for(std::list<OptNet::Edge>::iterator j = (*o)[i].begin(); j != (*o)[i].end(); ++j)
        {
            if(i < j->dest)
            {
                opedges[k++] = (GLuint) i;
                opedges[k++] = (GLuint) j->dest;
            }
        }
}

GraphWindow::~GraphWindow()
{

    if(vertices)
        delete [] vertices;
    if(edges)
        delete [] edges;
    if(nodes)
        delete [] nodes;
    if(colours)
        delete [] colours;
    if(keyVertices)
        delete [] keyVertices;
    if(opedges)
        delete [] opedges;
}
