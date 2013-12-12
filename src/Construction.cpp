#include "../include/Construction.h"
#include <limits>

std::size_t dropAlgorithm(SRGGraph& g, std::vector<bool>& relays, std::size_t threshold)
{
    std::size_t numRels(relays.size()), blackListed(0), terminals(0);
    for(int i=0; i<relays.size(); ++i)
        relays[i]=true;

    std::vector<std::size_t> relDegree(relays.size(),0);
    std::vector<bool> blacklist(relays.size(),false);

    for(int i=0; i<relays.size(); ++i)
        if(relays[i])
            for(std::list<SRGGraph::Edge>::iterator j = g[i].edges.begin(); j!= g[i].edges.end(); ++j)
                ++relDegree[j->dest];

    do
    {
        //find min (non blacklisted) degree - can I design a heap for this? er - yes! buckets
        std::size_t minVal = relays.size();
        std::size_t minDex = std::numeric_limits<size_t>::max();
        for(std::size_t i=0; i<relays.size(); ++i)
            if(relays[i] && !blacklist[i] && relDegree[i] < minVal)
            {
                minDex = i;
                minVal = relDegree[i];
            }

        if(minDex >= relays.size())
            break;

        //drop and see if connected
        relays[minDex] = false;

        if(g.biConnected(relays)) //but then it won't matter as this is O(n^3)
        {
            --numRels;
            for(std::list<SRGGraph::Edge>::iterator j = g[minDex].edges.begin(); j!= g[minDex].edges.end(); ++j)
                --relDegree[j->dest]; //decrease key?
        }
        else
        {
            relays[minDex] = true;
            blacklist[minDex] = true;
            ++blackListed;
        }
    }
    while(blackListed + threshold < numRels);
    return numRels;
}


