#include <iostream>
#include <fstream>
#include <Windows.h>

#include "../include/SRGGraph.h"
#include "../include/OptNet.h"
#include "../include/SRGAntColony.h"
#include "../include/Construction.h"



int main()
{
	int nodes[] = {50};
	int runs = 20;
	double mld = 0.2;
	double mtd = 0.3;
	double linkProb = 0.8;
	
	std::ofstream ofs;
	ofs.open("results.csv", std::ios::app);
	
	for(int type=0; type<sizeof(nodes)/sizeof(int); ++type)
	{
		ofs << "\n\nNodes," << nodes[type] << ",Drop,,ACO\n\n";
		std::cout << "\n\nNodes\t" << nodes[type] << "\tDrop\t\tACO\n\n";
		std::size_t n = nodes[type];
		
		int DropTot = 0;
		int ACOTot = 0;
		for(int instance = 0; instance < runs; ++instance)
		{
			OptNet* o=0;
			do
			{
				if(o)
					delete o;
				o = new OptNet(n,mtd);
				
				for(int i=0; i<n; ++i)
					for(int j=i+1; j<n; ++j)
						if(o->checkDistance(i,j,mld) && double(rand())/RAND_MAX < linkProb)
							o->AddEdge(i,j,o->getDistance(i,j));
			}
			while(!o->biConnected());
			
			SRGGraph g(n);
			o->GetSRGs(g);
			delete o;
			
			std::vector<bool> relays(n);
			
			unsigned long dropTime = GetTickCount();
			int dropResult = dropAlgorithm(g,relays);
			dropTime = GetTickCount() - dropTime;
		
			unsigned long ACOTime = GetTickCount();
			int ACOResult = SRGAntColony(g,relays,20,100);
			ACOTime = GetTickCount() - ACOTime;
			
			ofs << ",," << dropResult << "," << dropTime << "," << ACOResult << "," << ACOTime << "\n";
			std::cout << "\t\t" << dropResult << "\t" <<  dropTime/1000.f << "\t" << ACOResult << "\t" << ACOTime/1000.f << "\n";
			
			DropTot += dropResult;
			ACOTot += ACOResult;
		}
		
		ofs << "\nMean,," << double(DropTot)/runs << ",," << double(ACOTot)/runs << ",,Improvement," << 100 * (double(DropTot)/runs - double(ACOTot)/runs)/(double(DropTot)/runs)<< "\n";
		std::cout << "\nMean\t\t" << double(DropTot)/runs << "\t\t" << double(ACOTot)/runs  << "\t\tImprovement\t" << 100 * (double(DropTot)/runs - double(ACOTot)/runs)/(double(DropTot)/runs)<< "\n";		
	}
	
	ofs.close();

	return 0;
}