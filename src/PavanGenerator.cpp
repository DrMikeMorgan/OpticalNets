#include "../include/PavanGenerator.h"
#include <cmath>

namespace mikeNets{

	struct Edgy
	{
		Edgy(std::size_t _i, std::size_t _j, double _wax):i(_i),j(_j),wax(_wax){} 
		std::size_t i,j;
		double wax;
		bool operator < (const Edgy& e)  const {return wax < e.wax;} 
	};

	bool PavanGenerator::validPosition(std::size_t i)
	{
		for(int j=0; j<i; ++j)
		    if(!checkDistance(i,j))
		        return false;
		return true;
	}

	bool PavanGenerator::checkDistance(std::size_t i, std::size_t j)
	{
		return ((xcoords[i]-xcoords[j])*(xcoords[i]-xcoords[j]) + (ycoords[i]-ycoords[j])*(ycoords[i]-ycoords[j])) > _d*_d;
	}

	double PavanGenerator::getDistance(std::size_t i, std::size_t j)
	{
		return sqrt((xcoords[i]-xcoords[j])*(xcoords[i]-xcoords[j]) + (ycoords[i]-ycoords[j])*(ycoords[i]-ycoords[j]));
	}

	void PavanGenerator::Generate(OptNet *& o, double mtd)
	{		

		if(o)
		    delete o;
		o = new OptNet(_n,mtd);

		std::vector<std::vector<bool> > adjMatrix(_n, std::vector<bool>(_n,false));

		int p1(0),p2; //p1 = largest prime by which _r is divisible, p2 result of division
		for(int i=2; i<=_r/2; ++i)
		{
		    bool prime = true;
		    for(int j=2; j<i; j++)
		    {
		        if(i%j == 0)
		        {
		            prime = false;
		            break;
		        }
		    }
		    if(prime && _r%i == 0)
		    {
		        p1 = i;
		        p2 = _r/i;
		    }
		} //phew!!

		regions.resize(p1,std::vector<Region>(p2,Region(this)));
		//calculate n_max
		std::size_t n_max = (1.f/p1 * 1.f/p2)/(_d*_d);

		//set region boundaries
		for(int i=0; i<p1; ++i)
		{
		    double y = i * 1.f/p1;
		    for(int j=0; j<p2; ++j)
		    {
		        double x = j * 1.f/p2;
		        regions[i][j].box(x,y,1.f/p2,1.f/p1);
				regions[i][j].i = i;
				regions[i][j].j = j;
				//set neighbour regions
				if(i)
					regions[i][j].neighbours.push_back(&(regions[i-1][j]));
				if(j)
					regions[i][j].neighbours.push_back(&(regions[i][j-1]));
				if(i*j)
					regions[i][j].neighbours.push_back(&(regions[i-1][j-1]));	//diagonal neighbour
				if(i!=p1-1)
					regions[i][j].neighbours.push_back(&(regions[i+1][j]));
				if(j!=p2-1)
					regions[i][j].neighbours.push_back(&(regions[i][j+1]));
				if(i!=p1-1 && j!=p2-1)
					regions[i][j].neighbours.push_back(&(regions[i+1][j+1])); //other diagonal neighbour
		    }
		}

		//assign nodes to regions, randomly
		for(int i=0; i<_n; ++i)
		{
		    std::size_t ver;
		    std::size_t hor;
		    do
		    {
		        ver = rand()%p1;
		        hor = rand()%p2;
		    }
		    while(regions[ver][hor].size()==n_max);

		    do
		    {
		        xcoords[i] = rand()/(double(p2)*RAND_MAX) + regions[ver][hor]._x;
		        ycoords[i] = rand()/(double(p1)*RAND_MAX) + regions[ver][hor]._y;
		    }
		    while(!validPosition(i));
		    regions[ver][hor].push_back(i);
		}


		//connect within regions
		for(std::size_t i=0; i<p1; ++i)
		    for(std::size_t j=0; j<p2; ++j)
		    {
				regions[i][j].gateways.resize(regions[i][j].size(),false);
		        if(regions[i][j].size()==2)
		        {
		            o->AddEdge(regions[i][j][0],regions[i][j][1],getDistance(regions[i][j][0],regions[i][j][1]));
					adjMatrix[regions[i][j][0]][regions[i][j][1]] = adjMatrix[regions[i][j][1]][regions[i][j][0]] = true;
		        }
		        else if(regions[i][j].size()==3)
		        {
		            o->AddEdge(regions[i][j][0],regions[i][j][1],getDistance(regions[i][j][0],regions[i][j][1]));
					adjMatrix[regions[i][j][0]][regions[i][j][1]] = adjMatrix[regions[i][j][1]][regions[i][j][0]] = true;
		            o->AddEdge(regions[i][j][1],regions[i][j][2],getDistance(regions[i][j][1],regions[i][j][2]));
					adjMatrix[regions[i][j][1]][regions[i][j][2]] = adjMatrix[regions[i][j][2]][regions[i][j][1]] = true;
		            o->AddEdge(regions[i][j][0],regions[i][j][2],getDistance(regions[i][j][0],regions[i][j][2]));
					adjMatrix[regions[i][j][0]][regions[i][j][2]] = adjMatrix[regions[i][j][2]][regions[i][j][0]] = true;
		        }
		        else if(regions[i][j].size()>3)
		        {
		            std::vector<bool> used(regions[i][j].size(),false);
		            int numUsed = 1;
		            std::size_t first;
		            std::size_t curr = first = rand()%regions[i][j].size();   //can improve to add from both ends
		            while(numUsed<regions[i][j].size())
		            {
		                //select next node q using Waxman prob
		                //(P(q) = beta*exp(getDistance
                        used[curr] = true;
                        ++numUsed;
                        std::size_t q = regions[i][j].size();
                        double maxWax = std::numeric_limits<double>::min();
                        //loop to find minimum Wax Prob
                        for(std::size_t l=0; l<regions[i][j].size(); ++l)
                        {
                            if(used[l])
                                continue;
                            double waxProb = _beta*exp( (0.f-getDistance(regions[i][j][curr],regions[i][j][l]))/(_alpha*sqrt( (1.f/p1*1.f/p1)+(1.f/p2*1.f/p2) )) );
                            if(waxProb>maxWax)
                            {
                                q = l;
                                maxWax = waxProb;
                            }
                        }
		                o->AddEdge(regions[i][j][curr],regions[i][j][q],getDistance(regions[i][j][curr],regions[i][j][q]));
						adjMatrix[regions[i][j][curr]][regions[i][j][q]] = adjMatrix[regions[i][j][q]][regions[i][j][curr]] = true;
                        curr = q;
		            }
		            o->AddEdge(regions[i][j][curr],regions[i][j][first],getDistance(regions[i][j][curr],regions[i][j][first]));
					adjMatrix[regions[i][j][curr]][regions[i][j][first]] = adjMatrix[regions[i][j][first]][regions[i][j][curr]] = true;
		        }
		    }

			int minDegree = std::numeric_limits<int>::max();
			for(int i=0; i<_n; ++i)
				if((*o)[i].size() < minDegree)
					minDegree = (*o)[i].size();

			std::vector<std::vector<bool> > radj(p1*p2,std::vector<bool>(p1*p2,false));
			//interconnect regions
			for(std::size_t i=0; i<p1; ++i)
		    	for(std::size_t j=0; j<p2; ++j)			
				{
					if(regions[i][j].size() == 0)
						continue;
					for(int counter=regions[i][j].gateCnt; counter<2; ++counter)
					{
						double maxWax = std::numeric_limits<double>::min();
						std::size_t src, dest;
						Region* destReg = (Region*) 0;
						//iterate through neighbour regions
						for(std::list<Region*>::iterator reg = regions[i][j].neighbours.begin(); reg!=regions[i][j].neighbours.end(); ++reg)
						{
							if((*reg)->size() == 0 || radj[i*p2+j][(**reg).i * p2 + (**reg).j ] == true)
								continue;
							//iterate over all pairs of nodes in source and destination region
							for(std::size_t srcNode = 0; srcNode < regions[i][j].size(); ++srcNode)
							{
								if(regions[i][j].gateways[srcNode] && regions[i][j].size() > 1)
									continue;
								for(std::size_t destNode = 0; destNode < (*reg)->size(); ++destNode)
								{
									if((*reg)->gateways[destNode] && (*reg)->size() > 1)
										continue;
									double magicNumber = _beta*exp( (0.f-getDistance(regions[i][j][srcNode],(**reg)[destNode]))/(_alpha*sqrt(2.f)) );
									if(magicNumber > maxWax)
									{
										maxWax = magicNumber;
										destReg = *reg;
										src = srcNode;
										dest = destNode;
									}
								}
							}
						}
						if(destReg == (Region*) 0)
							continue;
						radj[i*p2+j][destReg->i * p2 + destReg->j ] = radj[destReg->i * p2 + destReg->j ][i*p2+j] = true; //connect regions
						o->AddEdge(regions[i][j][src], regions[destReg->i][destReg->j][dest], getDistance(regions[i][j][src],regions[destReg->i][destReg->j][dest])); //add the edge				
						adjMatrix[regions[i][j][src]][regions[destReg->i][destReg->j][dest]] = adjMatrix[regions[destReg->i][destReg->j][dest]][regions[i][j][src]] = true;
						regions[i][j].gateways[src] = true;
						++regions[i][j].gateCnt;
						destReg->gateways[dest] = true;
						++destReg->gateCnt;					
					}
					minDegree = std::numeric_limits<int>::max();
					for(int h=0; h<_n; ++h)
						if((*o)[h].size() < minDegree)
							minDegree = (*o)[h].size();
					
				}

		//sort edges by waxprob 

		std::vector<Edgy> edges; //i < j by convention
		edges.reserve(_n*_n/2);

		for(std::size_t i=0; i<_n; ++i)
			for(std::size_t j=i+1; j<_n; ++j)
				if(!adjMatrix[i][j])
					edges.push_back(Edgy(i,j, _beta*exp( (0.f-getDistance(i,j))/(_alpha*sqrt(2.f)) )));

		std::sort(edges.begin(), edges.end());
		std::reverse(edges.begin(), edges.end());

		for(std::size_t i=0; minDegree < _dmin && i<edges.size(); ++i)
		{
			//select any two nodes with minimum waxprob (must not already be connected)
			if((*o)[edges[i].i].size() < _dmax && (*o)[edges[i].j].size() < _dmax)
			{
				o->AddEdge(edges[i].i, edges[i].j, getDistance(edges[i].i,edges[i].j));
			}
			else
			{
				//std::cout << "Ignoring (" << edges[i].i << "," << edges[i].j << ",)[" << (*o)[edges[i].i].size() << "," << (*o)[edges[i].j].size() << "]\t";  
			}	
			minDegree = std::numeric_limits<int>::max();
			for(std::size_t j=0; j<_n; ++j)
				if((*o)[j].size() < minDegree)
					minDegree = (*o)[j].size();
			//std::cout << _dmax << "\n";
		}

		std::vector<float> coords(_n*2);
		for(std::size_t i=0; i<_n; ++i)
		{
		    coords[i*2] = (float) xcoords[i];
		    coords[i*2+1] = (float) ycoords[i];
		}
		o->setCoords(coords);
	}

	PavanGenerator::~PavanGenerator()
	{
		//dtor
	}

}
