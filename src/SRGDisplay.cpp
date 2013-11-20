#include "..\include\SRGDisplay.h"

#include <list>
#include <fstream>
#include <cstdlib>

SRGDisplay::SRGDisplay(int w, int h, SRGGraph & g) : graph(g), wi(w), hi(h)
{
    size = (GLuint) g.size();
    coords = new GLfloat[size*2];
    for(int i=0; i<size; ++i)
    {
        coords[i*2] = (GLfloat) g.getCoords()[i*2] * 2 * w - w;
        coords[i*2+1] = (GLfloat) g.getCoords()[i*2+1] * 2 * h - h;
    }
}

void SRGDisplay::getIndices(GLuint * ind)
{
    int k=0;
    for(GLuint i=0; i<size; ++i)
        for(std::list<SRGGraph::Edge>::iterator j = graph[i].edges.begin(); j != graph[i].edges.end(); ++j)
        {
            if(i < j->dest)
            {
                ind[k++] = i;
                ind[k++] = (GLuint) j->dest;
            }
        }
}

void SRGDisplay::resize(int w,int h)
{
    for(int i=0; i<size; ++i)
    {
        coords[i*2] = (GLfloat) graph.getCoords()[i*2] * 2 * w - w;
        coords[i*2+1] = (GLfloat) graph.getCoords()[i*2+1] * 2 * h - h;
    }
    wi=w;
    hi=h;
}
