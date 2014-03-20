#include "../include/PavanGenerator.h"
#include <cmath>

namespace mikeNets{

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
		        if(regions[i][j].size()==2)
		        {
		            o->AddEdge(regions[i][j][0],regions[i][j][1],getDistance(regions[i][j][0],regions[i][j][1]));
		        }
		        else if(regions[i][j].size()==3)
		        {
		            o->AddEdge(regions[i][j][0],regions[i][j][1],getDistance(regions[i][j][0],regions[i][j][1]));
		            o->AddEdge(regions[i][j][1],regions[i][j][2],getDistance(regions[i][j][1],regions[i][j][2]));
		            o->AddEdge(regions[i][j][0],regions[i][j][2],getDistance(regions[i][j][0],regions[i][j][2]));
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
                        double minWax = std::numeric_limits<double>::max();
                        //loop to find minimum Wax Prob
                        for(std::size_t l=0; l<regions[i][j].size(); ++l)
                        {
                            if(used[l])
                                continue;
                            double dist = _beta*exp( (0.f-getDistance(regions[i][j][curr],regions[i][j][l]))/(_alpha)); //sqrt( (1.f/p1*1.f/p1)+(1.f/p2*1.f/p2) )) );

                            if(dist<minWax)
                            {
                                q = l;
                                minWax = dist;
                            }
                        }
		                o->AddEdge(regions[i][j][curr],regions[i][j][q],getDistance(regions[i][j][curr],regions[i][j][q]));
                        curr = q;
		            }
		            o->AddEdge(regions[i][j][curr],regions[i][j][first],getDistance(regions[i][j][curr],regions[i][j][first]));
		        }
		    }

		std::vector<float> coords(_n*2);
		for(int i=0; i<_n; ++i)
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
