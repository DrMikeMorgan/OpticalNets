#include <iostream>
#include <fstream>
#include <cmath>
#include "include/PavanGenerator.h"
#include "include/Construction.h"

using namespace mikeNets;

bool dominationTest(SRGGraph& g, std::vector<bool> &relays)
{
	std::vector<size_t> domination(relays.size(), 0);
	for(int i=0; i<relays.size(); ++i)
	{
		if(relays[i])
		{
			for(std::list<SRGGraph::Edge>::iterator j = g[i].edges.begin(); j != g[i].edges.end(); ++j)
				++domination[j->dest];
		}
	}
	for(int i=0; i<relays.size(); ++i)
		if(domination[i] < 2)
			return false;
	return true;
}

bool SRGDomTest(SRGGraph& g, std::vector<bool> &relays)
{
	
	for(int i=0; i<relays.size(); ++i)
	{
		std::vector<size_t> domination(relays.size(), 0);
		std::vector<std::vector<bool> > matrix(relays.size(), std::vector<bool>(relays.size(),false));
		
		for(std::vector<SRGGraph::Edge*>::iterator j = g.getSRG(i).edges.begin(); j!=g.getSRG(i).edges.end(); ++j)
			matrix[(*j)->src][(*j)->dest] = matrix[(*j)->dest][(*j)->src] = true;

		for(int j = 0; j<relays.size(); ++j)
		{
			if(!relays[j] || j==i)
				continue;
			for(std::list<SRGGraph::Edge>::iterator k = g[j].edges.begin(); k != g[j].edges.end(); ++k)
				if(!matrix[j][k->dest])
					++domination[k->dest];
		}
		for(int j=0; j<relays.size(); ++j)
			if(j != i && domination[j] < 1)
			{
				return false;
			}
	}
	return true;
}

void DFS(size_t n, size_t exc, SRGGraph& g, std::vector<bool> &relays, std::vector<bool>& visited)
{
	visited[n] = true;
	if(n == exc)
		return;
	for(std::list<SRGGraph::Edge>::iterator j = g[n].edges.begin(); j!= g[n].edges.end(); ++j)
	{
		if(relays[j->dest] && !visited[j->dest])
		{
			DFS(j->dest, exc, g, relays, visited);	
		}
	}
}

bool biConTest(SRGGraph& g, std::vector<bool> &relays)
{
	//use the oldschool 'delete each node and test for connectivity' approach
	for(size_t i=0; i<relays.size(); ++i)
	{
		if(!relays[i])
			continue;										//no need to test under deletion of terminals
		size_t first;
		for(first=0; !relays[first] || first==i; ++first);	//find first relay node for DFS

		std::vector<bool> visited (relays.size(), false);

		DFS(first, i, g, relays, visited);

		for(size_t j=0; j < relays.size(); ++j)
			if(relays[j] && !visited[j])
			{
				return false;
			}
	}
	return true;
}

void SRGDFS(size_t n, size_t exc, SRGGraph& g, std::vector<bool> &relays, std::vector<bool>& visited, std::vector<std::vector<bool> > &matrix)
{
	visited[n] = true;
	if(n == exc)
		return;
	for(std::list<SRGGraph::Edge>::iterator j = g[n].edges.begin(); j!= g[n].edges.end(); ++j)
	{
		if(relays[j->dest] && !visited[j->dest] && !matrix[n][j->dest])
		{
			SRGDFS(j->dest, exc, g, relays, visited, matrix);	
		}
	}
}

bool SRGBiConTest(SRGGraph& g, std::vector<bool> &relays)
{
	
	for(int i=0; i<relays.size(); ++i)
	{
		std::vector<bool> visited(relays.size(), false);
		std::vector<std::vector<bool> > matrix(relays.size(), std::vector<bool>(relays.size(),false));
		
		for(std::vector<SRGGraph::Edge*>::iterator j = g.getSRG(i).edges.begin(); j!=g.getSRG(i).edges.end(); ++j)
			matrix[(*j)->src][(*j)->dest] = matrix[(*j)->dest][(*j)->src] = true;

		size_t first;
		for(first=0; !relays[first] || first==i; ++first);
		
		SRGDFS(first, i, g, relays, visited, matrix);		

		for(size_t j=0; j < relays.size(); ++j)
			if(j!=i && relays[j] && !visited[j])
			{
				/*std::cout << "Relay " << j << " not visited under deletion of " << i << "\n";
				std::cout << "Edge list: ";
				for(std::list<SRGGraph::Edge>::iterator k = g[j].edges.begin(); k!= g[j].edges.end(); ++k)
					if(relays[k->dest])
						std::cout << k->dest << (visited[k->dest]?"(visited)":"(not visited)");
				std::cout << "SRG: ";
				for(std::vector<SRGGraph::Edge*>::iterator k = g.getSRG(i).edges.begin(); k!=g.getSRG(i).edges.end(); ++k)
					std::cout << "(" << (*k)->src << "," << (*k)->dest << ")"; */
			
				return false;
			}
	}
	return true;
}

int main()
{
	std::ofstream dbg;
	
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

	PavanGenerator p(size, 3, 5, regions, 0.01, 0.4, 0.5, 1);

	for(size_t i=0; i<tests; ++i)
	{
		std::cout << "1";
		OptNet *o = (OptNet*) 0;
		std::cout << "1";
		do
		{
			p.Generate(o,mtd);
			std::cout <<".";
		}
		while(!o->biConnected());
		std::cout << "2";

		SRGGraph g(size);
		std::cout << "3";
		o->GetSRGs(g);
		std::cout << "4";

		//Run drop algorithm and test for final connectivity here...

		std::vector<bool> relays(size,true);
		std::cout << "5";
		int result = dropAlgorithm(g,relays);
		std::cout << "6\n";

		std::cout << "Test " << i << " of " << tests << "\n";
		for(size_t j=0; j<size; ++j)
			if(relays[j])
				std::cout << j << ":";
		std::cout << "\nresult = " << result << "\n";
		if(!dominationTest(g, relays))
			std::cout << "Domination Error!\n";
		if(!biConTest(g, relays))
			std::cout << "Biconnectivity Error!\n";
		if(!SRGDomTest(g, relays))
			std::cout << "SRG Domination Error!\n";
		if(!SRGBiConTest(g,relays))
			std::cout << "SRG Connectivity Error!\n";

		for(size_t blimp=0; blimp<size; ++blimp)
		{
			if(!relays[blimp])
				continue;
			relays[blimp] = false;
			if(dominationTest(g, relays) && biConTest(g, relays) && SRGDomTest(g, relays) && SRGBiConTest(g,relays))
			{
				std::cout << "False negatives in drop algorithm!\n";
				
			}
			relays[blimp] = true;
		}

		std::cout << "________________________________________________________________________________________________\n\n";
		delete o;
	}
	
	
	return 0;
}

