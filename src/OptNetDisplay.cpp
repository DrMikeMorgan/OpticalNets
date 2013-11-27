#include "../include/OptNetDisplay.h"

#include <list>


OptNetDisplay::OptNetDisplay(int w, int h, OptNet& g) : graph(g)
{
    size = (GLuint) g.size();
}

void OptNetDisplay::getIndices(GLuint * ind)
{
    int k=0;
    for(GLuint i=0; i<size; ++i)
        for(std::list<OptNet::Edge>::iterator j = graph[i].begin(); j != graph[i].end(); ++j)
        {
            if(i < j->dest)
            {
                ind[k++] = i;
                ind[k++] = (GLuint) j->dest;
            }
        }
}
