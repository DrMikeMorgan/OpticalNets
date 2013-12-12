#include "../include/SRGAntColony.h"
#include "../include/Construction.h"

std::size_t ACODrop(SRGGraph& g, std::vector<bool>& relays, std::vector<std::size_t>& dropped, PheromoneMatrix &p)
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
    int cur = relays.size();
    do
    {
        //choose next node using pheromone matrix
        float magicNumber = float(rand())/RAND_MAX;

        //make prefixes... tomorrow!
        std::vector<float> options;
        std::vector<std::size_t> indexes;
        options.reserve(relays.size());
        indexes.reserve(relays.size());
        float sum = 0;

        for(int i=0; i<relays.size();++i)
        {
            //do the whole selection in here (first choice exceptional case)...

            if(blacklist[i] || !relays[i])
                continue;
            if(numRels == relays.size())
                sum += p.init(i)/relDegree[i];
            else
                sum += p[cur][i]/relDegree[i];
            options.push_back(sum); //inclusive prefix sum
            indexes.push_back(i);
        }

        magicNumber *= sum;
        for(cur=0; magicNumber > options[cur]; ++cur);

        cur = indexes[cur];

        //drop and see if connected
        relays[cur] = false;

        if(g.biConnected(relays)) //but then it won't matter as this is O(n^3)
        {
            --numRels;
            for(std::list<SRGGraph::Edge>::iterator j = g[cur].edges.begin(); j!= g[cur].edges.end(); ++j)
                --relDegree[j->dest]; //decrease key?
            dropped.push_back(cur);
        }
        else
        {
            relays[cur] = true;
            blacklist[cur] = true;
            ++blackListed;
        }
    }
    while(blackListed < numRels);
    return numRels;
}

PheromoneMatrix::PheromoneMatrix(std::size_t n):std::vector<std::vector<float> >(n, std::vector<float>(n,1.f/n)), firstChoice(n,1.f/n)
{
    for(int i=0; i<size(); ++i)
        (*this)[i][i] = 0;
}


void PheromoneMatrix::update(std::vector<std::size_t>& nodes, float result)
{
    for(int i=0; i<nodes.size()-1; ++i)
        for(int j=0; j<size(); ++j)
            if(j == nodes[i+1])
                (*this)[nodes[i]][j] = (1-rho)*(*this)[nodes[i]][j] + Q/result;
            else
                (*this)[nodes[i]][j] = (1-rho)*(*this)[nodes[i]][j];
    for(int i=0; i<firstChoice.size(); ++i)
        if(i == nodes[0])
            firstChoice[i] = (1-rho)*firstChoice[i] + Q/result;
        else
            firstChoice[i] = (1-rho)*firstChoice[i];
}

int SRGAntColony(SRGGraph& g, std::vector<bool>& relays, std::size_t genSize, std::size_t iterations)
{
    PheromoneMatrix p(relays.size());
    p.setEvaporationRate(0.3);
    p.setQ(relays.size()*0.3);
    std::vector<bool> best(relays.size(),true);
    int opt = relays.size();
    for(int i=0; i<iterations; ++i)
    {
        std::vector<std::vector<std::size_t> > tours(genSize);
        std::vector<size_t> result(genSize);
        for(int j=0; j<genSize; ++j)
        {
            //some parallel code?
            tours[j].reserve(relays.size());
            result[j]=ACODrop(g,relays,tours[j],p);
            if(result[j] < opt)
            {
                std::copy(relays.begin(), relays.end(), best.begin());
                opt = result[j];
            }
        }
        for(int j=0; j<genSize; ++j)
            p.update(tours[j],(float) result[j]);
    }
    std::copy(best.begin(), best.end(), relays.begin());
    return opt;
}
