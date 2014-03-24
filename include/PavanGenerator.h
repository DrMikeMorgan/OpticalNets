#ifndef PAVANGENERATOR_H
#define PAVANGENERATOR_H

#include <vector>
#include <list>
#include "OptNet.h"

namespace mikeNets
{

	class PavanGenerator;

	class Region : public std::vector<std::size_t>
	{
		public:
		    Region(PavanGenerator * p) : _p(p), gateCnt(0){}
		    void box(double x,double y,double w,double h){_x=x;_y=y;_w=w;_h=h;}
			friend class PavanGenerator;
		protected:
		    PavanGenerator * _p;
		    double _x,_y,_w,_h;
			std::size_t gateCnt, i, j;
			std::list<Region*> neighbours;
			std::vector<bool> gateways;
	};

	class PavanGenerator
	{
		public:
		    PavanGenerator(std::size_t n,
		                   double dmin,
		                   double dmax,
		                   std::size_t r,
		                   double d,
		                   double alpha,
		                   double beta,
		                   std::size_t phi) : _n(n),_dmin(dmin),_dmax(dmax),_r(r),_d(d),_alpha(alpha),_beta(beta),_phi(phi),xcoords(n),ycoords(n){}
		    void Generate(OptNet *& o, double mtd);
		    virtual ~PavanGenerator();
		    friend class Region;
		protected:
		    std::size_t _n, _r, _phi;
		    double _d, _dmin, _dmax, _alpha, _beta;
		    std::vector<std::vector<Region> > regions;
		    std::vector<double> xcoords, ycoords;
		private:
		    bool validPosition(std::size_t i);
		    bool checkDistance(std::size_t i, std::size_t j);
		    double getDistance(std::size_t i, std::size_t j);
	};

}



#endif // PAVANGENERATOR_H
