#include "../include/Construction.h"
#include <limits>

std::size_t dropAlgorithm(SRGGraph& g, std::vector<bool>& relays)
{
    std::size_t numRels(0), blackListed(0), terminals(0);
    for(int i=0; i<relays.size(); ++i)
        if(relays[i])
            ++numRels;

    std::vector<std::size_t> relDegree(relays.size(),0);
    std::vector<bool> blacklist(relays.size(),false);

    for(int i=0; i<relays.size(); ++i)
        if(relays[i])
            for(std::list<SRGGraph::Edge>::iterator j = g[i].edges.begin(); j!= g[i].edges.end(); ++j)
                ++relDegree[j->dest];

    do
    {
        //find min (non blacklisted) degree - can I design a heap for this? er - yes! buckets
        std::size_t minDex = std::numeric_limits<size_t>::max();
        for(int i=0; i<relays.size(); ++i)
            if(relays[i] && !blacklist[i] && relDegree[i] < relDegree[minDex])
                minDex = i;

		std::cout << "Dropping " << minDex << "\t";

        //drop and see if connected
        relays[minDex] = false;

        if(g.biConnected(relays)) //but then it won't matter as this is O(n^3)
        {
            --numRels;
            for(std::list<SRGGraph::Edge>::iterator j = g[minDex].edges.begin(); j!= g[minDex].edges.end(); ++j)
                --relDegree[j->dest]; //decrease key?
			std::cout << "Dropped: relDegrees - ";
			for(int i=0; i<relays.size(); ++i)
				std::cout << (int) relDegree[i] << ", ";
			std::cout << "\n";
        }
        else
        {
            relays[minDex] = true;
            blacklist[minDex] = true;
            ++blackListed;
        }
    }
    while(blackListed < numRels);
}


