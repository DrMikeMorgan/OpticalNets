#include "../include/PavanGenerator.h"
#include <cmath>

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
            xcoords[i] = rand()/double(p2*RAND_MAX) + regions[ver][hor]._x;
            ycoords[i] = rand()/double(p1*RAND_MAX) + regions[ver][hor]._y;
        }
        while(!validPosition(i));
        regions[ver][hor].push_back(i);
    }

    //connect within regions
    for(int i=0; i<p1; ++i)
        for(int j=0; j<p2; ++j)
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
                for(int k=1; k<regions[i][j].size(); ++k)
                {
                    o->AddEdge(regions[i][j][k-1],regions[i][j][k],getDistance(regions[i][j][k-1],regions[i][j][k]));
                }
                o->AddEdge(regions[i][j][0],regions[i][j][regions[i][j].size()-1],getDistance(regions[i][j][0],regions[i][j][regions[i][j].size()-1]));
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
