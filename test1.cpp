#include <iostream>
#include <fstream>
#include <cmath>
#include "include/PavanGenerator.h"

using namespace mikeNets;

int main()
{
	size_t tests;
	std::cout << "Enter number of tests";
	std::cin >> tests;

	size_t size;
	std::cout << "Enter graph size";
	std::cin >> size;

	size_t regions;
	std::cout << "Enter number of regons";
	std::cin >> regions;

	double mtd;
	std::cout << "Enter MTD";
	std::cin >> mtd;

	PavanGenerator p(size, 3, 5, regions, 0.01, 0.4, 0.4, 1);
	

	for(size_t i=0; i<tests; ++i)
	{
		OptNet *o = (OptNet*) 0;
		p.Generate(o,mtd);
		std::cout << ".";
		
		//test3: check edge distances are correct after running Pavan graph generation

		for(size_t i=0; i<size; ++i)
			for(std::list<OptNet::Edge>::iterator j=(*o)[i].begin(); j != (*o)[i].end(); ++j)
			{
				float dx = o->getCoords()[i*2] - o->getCoords()[(j->dest)*2];
				float dy = o->getCoords()[i*2+1] - o->getCoords()[(j->dest)*2+1];
				double dist = sqrt(dx*dx + dy*dy);
				if(dist < j->length - 0.001 || dist > j->length + 0.001)
					std::cout << "Error: " << dist << ", " << j->length << "\n";
			}


		//----------------------------CODE FOR TEST BATCH 1: CHECK SRG CORRECTNESS-------------------------------

		//reverse test: bool matrix denotes if edge j,k is in SRG i
		/*std::vector<std::vector<bool> > srgMat(size,std::vector<bool>(size,false));
		//Compute this dynamically and identify SRG edges by iterating across getSRG(i)
		for(size_t i=0; i<size; ++i)
			for(size_t j=0; j<g.getSRG(i).edges.size(); ++j)
				srgMat[g.getSRG(i).edges[j]->src][g.getSRG(i).edges[j]->dest] = srgMat[g.getSRG(i).edges[j]->dest][g.getSRG(i).edges[j]->src] = true;
		
		o->FloydWarshall();
		std::vector<std::vector<double> > matrix(size, std::vector<double>(size));
		for(size_t i=0; i<size; ++i)
			for(size_t j=0; j<size; ++j)
				matrix[i][j] = o->matrix[i][j];

		for(size_t i=0; i<size; ++i)
		{
			//THEN perform second FW pass with i removed, iterate across both matrices, searching for entries matrix[j][k] <= mtd and o->matrix[j][k] > mtd
			//If you find one not marked as TRUE in the boolean matrix above, flag the error.
			//That's tomorrow AM's plan and I is stickin to it!!! (God willing)
			o->FloydWarshall(i);
			for(int j=0; j<size; ++j)
				for(int k=0; k<size;++k)
					if(matrix[j][k] <=mtd && o->matrix[j][k]>mtd && !matrix[j][k])
						std::cout << "Error: " << j << "->" << k << ": " << matrix[j][k] << ", " << o->matrix[j][k]<< "\n";
			/*for(size_t j = 0; j< g.getSRG(i).edges.size(); ++j)
			{
				if(o->matrix[g.getSRG(i).edges[j]->src][g.getSRG(i).edges[j]->dest] <= mtd || matrix[g.getSRG(i).edges[j]->src][g.getSRG(i).edges[j]->dest] > mtd)
						std::cout << g.getSRG(i).edges[j]->src << "->" << g.getSRG(i).edges[j]->dest << ": " << o->matrix[g.getSRG(i).edges[j]->src][g.getSRG(i).edges[j]->dest] << ", " << matrix[g.getSRG(i).edges[j]->src][g.getSRG(i).edges[j]->dest] << "\t";			
				debug << "/";			
			}		
		}*/	
		delete o;	
	}
	
	return 0;
}

