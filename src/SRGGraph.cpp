#include "../include/SRGGraph.h"
#include <fstream>
#include <iostream>

void spit(int i)
{
	std::ofstream dbg;
	dbg.open("debug.txt",std::ios::out);
	dbg << i;
	dbg.close();
}

namespace mikeNets{

	typedef std::list<SRGGraph::Edge>::iterator edgeIterator;

	void SRGGraph::init()
	{
		    nodes = new Node[n];
		    for(int i=0; i<n; ++i)
		    {
		        nodes[i].SRGID = -1;
		        nodes[i].active = true;
		    }
		    coords = new float[n*2];
		    SRGs = new SingleRiskGroup[n];
	}

	void SRGGraph::AddEdge(int i, int j, int SRG)
	{
		for(edgeIterator e = nodes[i].edges.begin(); e != nodes[i].edges.end(); ++e)
			if(e->dest == j)
				return;

		for(edgeIterator e = nodes[j].edges.begin(); e != nodes[j].edges.end(); ++e)
			if(e->dest == i)
				return;

		if(SRG != -1)
		{
		    nodes[i].edges.push_back({i,j,true,2});
		    nodes[j].edges.push_back({j,i,true,2});
		    SRGs[SRG].edges.push_back(&(nodes[i].edges.back()));
		    SRGs[SRG].edges.push_back(&(nodes[j].edges.back()));
		}
		else
		{
		    nodes[i].edges.push_back({i,j,true,1});
		    nodes[j].edges.push_back({j,i,true,1});
		}
		++m;
	}

	void SRGGraph::AddToSRG(int i, int j, int SRG)
	{
		for(std::vector<Edge*>::iterator k=SRGs[SRG].edges.begin(); k!= SRGs[SRG].edges.end(); ++k)
			if((*k)->src == i && (*k)->dest == j || (*k)->src == j && (*k)->dest == i)
				return;
		edgeIterator e1,e2;
		for(e1 = nodes[i].edges.begin(); e1!=nodes[i].edges.end(); ++e1)
		    if(e1->dest == j)
		        break;
		for(e2 = nodes[j].edges.begin(); e2!=nodes[j].edges.end(); ++e2)
		    if(e2->dest == i)
		        break;
		if(e1 != nodes[i].edges.end() && e2!=nodes[j].edges.end())
		{
		    SRGs[SRG].edges.push_back(&(*e1));
		    SRGs[SRG].edges.push_back(&(*e2));
		    ++(e1->risk);
		    ++(e2->risk);
		}
	}

	void SRGGraph::reserveSRGs(size_t num)
	{
		if(SRGs)
			delete [] SRGs;
		SRGs = new SingleRiskGroup[num];
	}

	void SRGGraph::disableSRG(int SRG)
	{
		for(int j=0; j<SRGs[SRG].nodes.size(); ++j)
		    nodes[SRGs[SRG].nodes[j]].active = false;
		for(int j=0; j<SRGs[SRG].edges.size(); ++j)
		    SRGs[SRG].edges[j]->active = false;
	}

	void SRGGraph::enableSRG(int SRG)
	{
		for(int j=0; j<SRGs[SRG].nodes.size(); ++j)
		    nodes[SRGs[SRG].nodes[j]].active = true;
		for(int j=0; j<SRGs[SRG].edges.size(); ++j)
		    SRGs[SRG].edges[j]->active = true;
	}

	double SRGGraph::evaluate(std::vector<bool> & relays)
	{
		int r = 0;
		for(int i=0; i<n; ++i)
		    if(relays[i])
		        ++r;

		int risk = 0;
		for(int i=0; i<n; ++i)
		    if(relays[i])
		        for(edgeIterator j=nodes[i].edges.begin(); j!=nodes[i].edges.end(); ++j)
		            if(relays[j->dest] && i < j->dest)
		                risk += j->risk;

		return r + static_cast<double>(risk)/(n*m);
	}

	std::ostream& operator << (std::ostream& o, SRGGraph g)
	{
		for(int i=0; i<g.n; ++i)
		{
		    if(!g.nodes[i].active)
		       continue;
		    o << "Node " << i << "(" << g.nodes[i].SRGID <<"): ";
		    for(std::list<SRGGraph::Edge>::iterator j = g.nodes[i].edges.begin(); j!= g.nodes[i].edges.end(); ++j)
		        if(j->active)
		            o << j->dest << "(" << j->risk << ")";
		    o << "\n";
		}
		return o;
	}

	bool SRGGraph::connected()
	{
		std::vector<bool> marks(n,false);
		int firstnode;
		for(firstnode = 0; !nodes[firstnode].active; ++firstnode);
		DFS(firstnode,marks);
		for(int i=0; i<n; ++i)
		    if(!marks[i] && nodes[i].active)
		        return false;
		return true;
	}

	void SRGGraph::clear()
	{
		for(int i=0; i<n; ++i)
		{
		    nodes[i].active = true;
		    nodes[i].SRGID = -1;
		    nodes[i].edges.clear();
		}
		SRGnum = 0;
	}

	void SRGGraph::DFS(int node, std::vector<bool>& marks)
	{
		marks[node] = true;
		for(std::list<Edge>::iterator j=nodes[node].edges.begin(); j!=nodes[node].edges.end();++j)
		    if(j->active && !marks[j->dest] && nodes[j->dest].active)
		        DFS(j->dest,marks);
	}

	bool SRGGraph::biConnected()
	{
		std::vector<int> dfsnum(n,n), low(n,n);
		if(!LPTRec(0,dfsnum,low,0,0))
		    return false;
		return true;
	}


	bool SRGGraph::biConnected(std::vector<bool>& relays)
	{
		//find first relay
		std::size_t first = 0;
		std::size_t numRels = 0;
		for(;!relays[first] && first < relays.size();++first);

		if(first == relays.size())
		{
			std::cout << "N";
		    return false; //no relays - does this mean it's biconnected or not? I'm saying no
		}

		//set non-relays inactive
		for(int i=0; i<relays.size(); ++i)
        {
		    nodes[i].active = relays[i];	//prevents connectivity algorithms from access to neighbour lists during searches, restricts search to relay component. Is it necessary? No it's dangerous
		    if(relays[i])
                numRels++;					//Count the number of relays
        }
		std::vector<int> dfsnum(n,n), low(n,n);
		if(!LPTRec(first,dfsnum,low,0,0)) 
		{
			std::cout << "B";
		    return false;			//Shouldn't need this with relComp->biConnected
		
		}
        //better plan: create relay component with SRGs and test THIS for connectivity

        SRGGraph * relComp = new SRGGraph(numRels);		//use numRels to alloc for relay component
        std::size_t relCnt = 0;							//not sure what this does yet, counts the number of relays ADDED to the component during construction
        std::vector<std::size_t>  relMap(relays.size(),relays.size()), srgMap(relays.size(),relays.size());		//Maps index of relay in G to index in relay component, idex of SRG in G to idx in relComp
        for(int i=0; i<relays.size(); ++i)
        {
            if(relays[i])
                relMap[i] = relCnt++;		//completes the mapping
        }


		//Add edges to relComp, transforming idxs using relMap
        for(int i=0; i<relays.size(); ++i)
            for(std::list<Edge>::iterator j=nodes[i].edges.begin(); j!=nodes[i].edges.end();++j)
                if(j->src<j->dest && relays[j->src] && relays[j->dest])		//make sure to add each edge only once!
                    relComp->AddEdge(relMap[j->src], relMap[j->dest]);


        
		//check srgs to see which affect the relay component
        std::vector<bool> RelevantSRGs(relays.size(),false);			//records SRGs which affect the Biconnectivity calculation
        std::vector<bool> DomRelevantSRGs(relays.size(),false);		//records SRGs which affect the 2-Domination calculation
        size_t srgCnt = 0;		//used for mapping SRGs
        for(size_t srg=0; srg<relays.size(); ++srg)
        {
            for(size_t edge=0; edge<SRGs[srg].edges.size(); ++edge)
            {
                if(relays[SRGs[srg].edges[edge]->src]&&relays[SRGs[srg].edges[edge]->dest])
                {
                    RelevantSRGs[srg]=true;		//if any edge belongs to relComp, SRG is relevant
                }
                else if(relays[SRGs[srg].edges[edge]->src]||relays[SRGs[srg].edges[edge]->dest])
                    DomRelevantSRGs[srg] = true;	//edge only relevant to Dom calc if one 'end' is a relay
            }
            if(RelevantSRGs[srg])
            {
                srgMap[srg] = srgCnt++;
                relComp->AddSRG();		//increment no of SRGs in graph
            }
        }
		relComp->reserveSRGs(srgCnt);


        for(size_t srg=0; srg<relays.size(); ++srg)
        {
            if(RelevantSRGs[srg])
            {
                if(relays[srg])
				{
                    relComp->AddToSRG(relMap[srg], srgMap[srg]);	//add relay to SRG (if and only if SRG is based on relay deletion)		
				}
                for(size_t edge=0; edge<SRGs[srg].edges.size(); ++edge)
				{
                    if(relays[SRGs[srg].edges[edge]->src]&&relays[SRGs[srg].edges[edge]->dest])
                    {
					    relComp->AddToSRG(relMap[SRGs[srg].edges[edge]->src],relMap[SRGs[srg].edges[edge]->dest],srgMap[srg]);	//add edges to SRG for relay component
					}				
				}				
            }
        }

		for(int i=0; i<srgCnt;++i)
		{
		    relComp->disableSRG(i);
		    if(!relComp->connected())
			{
				std::cout << "S";
		        return false;
		    }
			relComp->enableSRG(i);
		}


        //Domination check should remain
		//check domination (may want to do this another way?)
		for(size_t i=0; i<relays.size(); ++i)
		{
		    if(!relays[i])	//check for non-relays only
		    {
		        size_t domCount = 0;
		        for(edgeIterator j = nodes[i].edges.begin(); j!= nodes[i].edges.end(); ++j)
		            if(relays[j->dest])
		                ++domCount;		//increment for each neighbouring relay
		        if(domCount < 2)
				{
					std::cout << "D";
		            return false;		//must have 2 or more relay neighbours
				}
		    }
		}

		for(size_t i=0; i<SRGnum;++i)
		{

		     //could move this disable/enable pair inside if statement for minor efficiency improvement
		    //check domination

		    if(DomRelevantSRGs[i])	//only test relevant SRGs
            {
				disableSRG(i);
                for(size_t j=0; j<relays.size(); ++j)
				{
                    if(!relays[j] && j != i)
                    {
                        size_t domCount = 0;
                        for(edgeIterator k = nodes[j].edges.begin(); k!= nodes[j].edges.end(); ++k)
						{
                            if(k->active && relays[k->dest] && k->dest!=i)	
                                ++domCount;
							if(!k->active)
							{
								bool checkActivity = false;
								for(size_t h=0; h<SRGs[i].edges.size(); ++h)
								{
									if((SRGs[i].edges[h]->src == j && SRGs[i].edges[h]->dest == k->dest) || (SRGs[i].edges[h]->src == k->dest && SRGs[i].edges[h]->dest == j) )
										checkActivity = true;
								}
								if(!checkActivity)
									std::cout << "!";
							}
						}
                        if(domCount < 1)
						{
							std::cout << "s";
							enableSRG(i);
                            return false;
						}
                    }
				}
				enableSRG(i);
            }
		}

		delete relComp;

		for(size_t i=0; i<relays.size(); ++i)
		    nodes[i].active = true;
		
		return true;
	}

	bool SRGGraph::LPTRec(int v, std::vector<int>& dfsnum, std::vector<int>& low, int cur, int parent)
	{
		dfsnum[v] = cur;
		++cur;
		int degree = 0;
		low[v] = dfsnum[v];
		for(std::list<Edge>::iterator j = nodes[v].edges.begin(); j!=nodes[v].edges.end();++j)
		{
		    if(!nodes[j->dest].active)
		        continue;
			if(dfsnum[j->dest] == n)
			{
		        if(!LPTRec(j->dest, dfsnum, low, cur, v))
					return false;
				if(low[j->dest] < low[v])
					low[v] = low[j->dest];
				if(low[j->dest] >= dfsnum[v] && dfsnum[v] != 0)
					return false;
				degree++;
			}
			else
				if(dfsnum[j->dest] < low[v] && j->dest != parent)
					low[v] = low[v]<dfsnum[j->dest] ? low[v] : dfsnum[j->dest];
		}

		if (dfsnum[v] == 0 && degree > 1)
			return false;
		return true;
	}

	/*bool SRGGraph::biConFast()
	{
		std::vector<int> dfsnum(n,n), low(n,n);
		std::vector<bool> SRGvec(n,false);
		lptlist lst;
		if(!LPTSRG(0,dfsnum,low,lst,SRGvec,0,0))
		    return false;
	}*/

	/*bool SRGGraph::LPTSRG(int v, std::vector<int>& dfsnum, std::vector<int>& low, lptlist& SRGlow, std::vector<bool>& SRG, int cur, int parent)
	{
		dfsnum[v] = cur;
		cur++;
		int degree = 0;
		low[v] = dfsnum[v];OptNet::
		for(lptlist::iterator j = SRGlow.begin(); j != SRGlow.end(); ++j)
		    j->second[v]=dfsnum[v];
		lptlist::iterator ere;
		bool SRGAdded = false;
		for(edgeIterator j = nodes[v].edges.begin(); j!=nodes[v].edges.end();++j)
		{
			if(dfsnum[j->dest] == n)
		    {
		        std::cout << "Tree edge from " << v << " to " << j->dest << "\n";
				if(j->SRGID != -1 && !SRG[j->SRGID])
		        {
					SRGlow.push_back(std::make_pair(j->SRGID,std::vector<int>(n,n)));//sort this right out!!!!
					SRGAdded = true;
					SRG[j->SRGID] = true;
					ere = SRGlow.end();
					--ere; //test this with simple graph
					std::cout << "Begin SRG " << j->SRGID << "\n";
				}
				if(!LPTSRG(j->dest, dfsnum, low, SRGlow, SRG, cur, v))
					return false;
				if(low[j->dest] < low[v])
		        {
					low[v] = low[j->dest];
					std::cout << v << " lpt changed to " << low[v] << "\n";
		        }
				if(low[j->dest] >= dfsnum[v] && dfsnum[v] != 1)
				{
					return false;
				}
				degree++;

				for(lptlist::iterator s = SRGlow.begin(); s!=SRGlow.end(); ++s)
		        {
		            if(j->SRGID != s->first && s->second[j->dest] < s->second[v])
		            {
		                s->second[v] = s->second[j->dest];  //update each SRG lpt if u,v not in SRG
		                std::cout << v << " lpt changed to " << s->second[v] << " SRG " << j->SRGID << "\n";
		            }
		            if(j->SRGID == s->first && s->second[j->dest] >= dfsnum[v] && dfsnum[v] != 1)
					{
						return false;
					}
		        }
		    }
			else
		    {
				if(dfsnum[j->dest] < low[v] && j->dest != parent)
		        {
					low[v] = low[v]<dfsnum[j->dest] ? low[v] : dfsnum[j->dest];
					std::cout << "Frond from " << v << " to " << j->dest << "\n";
				}
				for(lptlist::iterator s = SRGlow.begin(); s!=SRGlow.end(); ++s)
		            if(j->SRGID != s->first)
		            {
		                s->second[v] = dfsnum[j->dest];
		                std::cout << "Contributes to lpt change for SRG " << s->first << "\n";
		            }
		    }
			if(SRGAdded)
		    {
				SRGlow.erase(ere,SRGlow.end());
				SRG[j->SRGID] = false;
		    }
		}

		if(dfsnum[v] == 0 && degree > 1)
			return false;
		return true;
	}*/

	bool SRGGraph::checkInvariant(std::ostream& err)
	{
		//Check all edge entries i,j & j,i are equivalent
		for(int i=0; i<n; ++i)
		{
		    for(std::list<Edge>::iterator j = nodes[i].edges.begin(); j != nodes[i].edges.end(); ++j)
		    {
		        std::list<Edge>::iterator opp;
		        for(opp = nodes[j->dest].edges.begin(); opp != nodes[j->dest].edges.end(); ++opp)
		            if(opp->dest == i) break;
		        if(opp == nodes[j->dest].edges.end())
		        {
		            err << "No matching edge for " << i << "," << j->dest << "\n";
		            return false;
		        }
		        if(opp->active != j->active)
		        {
		            err << "Active state mismatch for edge " << i << "," << j->dest << "\n";
		            return false;
		        }
		        if(opp->risk != j->risk)
		        {
		            err << "Risk mismatch for edge " << i << "," << j->dest << "\n";
		            return false;
		        }
		    }
		}

		//Check SRG members all have same active state
		for(int i=0; i<SRGnum;++i)
		{
		    for(int j=1; j<SRGs[i].nodes.size(); ++j)
		        if(nodes[SRGs[i].nodes[j-1]].active != nodes[SRGs[i].nodes[j]].active)
		        {
		            err << "SRG with contradictory active states: " << 1 << "\n";
		            return false;
		        }
		    for(int j=1; j<SRGs[i].edges.size(); j+=2)
		    {
				//find edge in both edge lists
				edgeIterator e1, e2;
				for(e1 = nodes[SRGs[i].edges[j]->src].edges.begin(); e1->src != SRGs[i].edges[j]->src || e1->dest != SRGs[i].edges[j]->dest; ++e1);
				for(e2 = nodes[SRGs[i].edges[j]->dest].edges.begin(); e2->src != SRGs[i].edges[j]->dest || e2->dest != SRGs[i].edges[j]->src; ++e2);
					
		        if(SRGs[i].edges[j]->active != e1->active || SRGs[i].edges[j]->active != e2->active)
		        {
		            err << "SRG with contradictory active states: " << 2 << "\n";
		            return false;
		        }
		    }
		    /*if(SRGs[i].nodes.size() != 0 && SRGs[i].edges.size() != 0 && nodes[SRGs[i].nodes[0]].active != SRGs[i].edges[0]->active)
		    {
		        err << "SRG with contradictory active states: " << 3 << "\n";
		        return false;
		    }*/
		}

		//Consistency of SRGIDs for node list
		for(int i=0; i<n; ++i)
		{
		    if(nodes[i].SRGID!=-1)
		    {
		        bool found = false;
		        for(int j=0; j<SRGs[nodes[i].SRGID].nodes.size(); ++j)
		            if(SRGs[nodes[i].SRGID].nodes[j] == i)
		                found = true;
		        if(!found)
		        {
		            err << "Node in SRG " << nodes[i].SRGID << " not found in SRG node list\n";
		            return false;
		        }
		    }
		}
		return true;
	}
}
