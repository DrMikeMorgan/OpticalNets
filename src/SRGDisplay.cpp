#include "..\include\SRGDisplay.h"

#include <list>
#include <fstream>
#include <cstdlib>
#include <cmath>

SRGDisplay::SRGDisplay(int w, int h, SRGGraph & g) : graph(g), wi(w), hi(h)
{
    size = (GLuint) g.size();
    coords = new GLfloat[size*2];
    colours = new GLfloat[size*4];

    float offsets [] = {0,1,2};
    float baseCol [] = {1.0,1.0,1.0};
    float baseWeight = 0.7;
    for(int i=0; i<size; ++i)
    {
        coords[i*2] = (GLfloat) g.getCoords()[i*2] * 2 * w - w;
        coords[i*2+1] = (GLfloat) g.getCoords()[i*2+1] * 2 * h - h;
    }
    for(int i=0; i<size; ++i)
    {
        for(int j=0; j<3; ++j)
            colours[i*4+j] = ( (baseWeight*baseCol[j]) + ( (2-baseWeight) * GLfloat(rand())/RAND_MAX) )/2;
        colours[i*4+3] = 0.9;
        /*for(int j=0; j<3; ++j)
            colours[i*4+j] = (GLfloat)  fmod(0.618033988749895 * (offsets[j] + i), 1.0);
        colours[i*4+3] = 0.9;*/
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
