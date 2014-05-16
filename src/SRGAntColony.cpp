#include "../include/SRGAntColony.h"
#include "../include/Construction.h"
#include <pthread.h>
#include <fstream>
#include <iterator>

namespace mikeNets{

    class RanGen
    {
        unsigned long m, a, c, s;
    public:
        RanGen(unsigned long seed, unsigned long mod=2147483648,  unsigned long mul=1103515245, unsigned long inc = 12345):m(mod),a(mul),c(inc),s(seed){}
        unsigned int get() {s = (a*s+c)%m; return s & 0x3fffffff;}
    };

    const int NUM_THREADS = 4;

	std::size_t ACODrop(SRGGraph& g, std::vector<bool>& relays, std::vector<std::size_t>& dropped, PheromoneMatrix &p, RanGen &gen)
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
		    float magicNumber = float(gen.get())/0x3fffffff;

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

	struct ACOInfo
	{
	    ACOInfo(SRGGraph& _g, std::vector<bool> * _relays, std::vector<std::size_t> * _dropped, PheromoneMatrix * _p, RanGen * _r, size_t * _result):
                    g(_g), relays(_relays), dropped(_dropped), p(_p), r(_r), result(_result) {}
        SRGGraph g;
        std::vector<bool> * relays;
        std::vector<std::size_t> * dropped;
        PheromoneMatrix * p;
        RanGen * r;
        size_t * result;
	};

	void * DropThread(void * data)
	{
	    ACOInfo * a = static_cast<ACOInfo*>(data);
	    *(a->result) = ACODrop(a->g, *(a->relays), *(a->dropped), *(a->p), *(a->r));
	    pthread_exit(NULL);
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
		std::vector<bool> best(relays.size(),true), oldBest(relays.size(),true);
		int opt = relays.size();
		//put a proper hating criterion in
		bool halt = false;

        std::vector<RanGen*> generators(NUM_THREADS);
        for(int i=0; i<NUM_THREADS; ++i)
            generators[i] = new RanGen(rand());
		for(int i=0; !halt || i<iterations; ++i)
		{
		    std:copy(best.begin(), best.end(), oldBest.begin());
		    std::vector<std::vector<std::size_t> > tours(genSize, std::vector<std::size_t>(0));
		    std::vector<size_t> result(genSize);
		    for(int j=0; j<genSize/NUM_THREADS; ++j)
		    {
		        //some parallel code?
		        pthread_t threads[NUM_THREADS];
		        ACOInfo * infos[NUM_THREADS];
		        std::vector<std::vector<bool> > relayArray(NUM_THREADS, std::vector<bool>(relays.size(),true));
		        for(int k=0; k<NUM_THREADS; ++k)
                {
                    tours[j*NUM_THREADS+k].reserve(relays.size());
                    infos[k] = new ACOInfo(g,&relayArray[k],&tours[j*NUM_THREADS+k],&p, generators[k], &result[j*NUM_THREADS+k]);
                    int rc = pthread_create(&threads[k], NULL, DropThread, (void *) infos[k]);
                }
                for(int k=0; k<NUM_THREADS; ++k)
                    pthread_join(threads[k],NULL);
                for(int k=0; k<NUM_THREADS; ++k)
                {
                    if(result[j*NUM_THREADS+k] < opt)
                    {
                        std::copy(relayArray[k].begin(), relayArray[k].end(), best.begin());
                        opt = result[j*NUM_THREADS+k];
                    }
                    delete infos[k];
                }
		    }
		    for(int j=0; j<genSize; ++j)
            {
		        p.update(tours[j],(float) result[j]);
            }
            halt = true;
            for(int j=0; j<best.size(); ++j)
                if(best[j]!=oldBest[j])
                    halt = false;
		}
		std::copy(best.begin(), best.end(), relays.begin());
		return opt;
	}

}
