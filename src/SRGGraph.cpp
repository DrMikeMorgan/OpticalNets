#include "..\include\SRGGraph.h"
typedef std::list<SRGGraph::Edge>::iterator edgeIterator;

void SRGGraph::AddEdge(int i, int j, int SRG)
{
    nodes[i].edges.push_back({j,true,SRG});
    nodes[j].edges.push_back({i,true,SRG});
    if(SRG != -1)
    {
            SRGs[SRG].edges.push_back(&(nodes[i].edges.back()));
            SRGs[SRG].edges.push_back(&(nodes[j].edges.back()));
    }
    ++m;
}

void SRGGraph::AddToSRG(int i, int j, int SRG)
{
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
    }
}

void SRGGraph::disableSRG(int SRG)
{
    for(int j=0; j<SRGs[SRG].nodes.size(); ++j)
        nodes[SRGs[SRG].nodes[j]].active = false;
    for(int j=0; j<SRGs[SRG].edges.size(); ++j)
        (SRGs[SRG].edges[j])->active = false;
}

void SRGGraph::enableSRG(int SRG)
{
    for(int j=0; j<SRGs[SRG].nodes.size(); ++j)
        nodes[SRGs[SRG].nodes[j]].active = true;
    for(int j=0; j<SRGs[SRG].edges.size(); ++j)
        (SRGs[SRG].edges[j])->active = true;
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
                o << j->dest << "(" << j->SRGID << ")";
        o << "\n";
    }
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
    //will need some additional lowpoint code!!!
    std::vector<int> dfsnum(n,n), low(n,n);
    if(!LPTRec(0,dfsnum,low,0,0))
        return false;

    for(int i=0; i<SRGnum;++i)
    {
        disableSRG(i);
        if(!connected())
            return false;
        enableSRG(i);
    }
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

bool SRGGraph::biConFast()
{
    std::vector<int> dfsnum(n,n), low(n,n);
    std::vector<bool> SRGvec(n,false);
    lptlist lst;
    if(!LPTSRG(0,dfsnum,low,lst,SRGvec,0,0))
        return false;
}

bool SRGGraph::LPTSRG(int v, std::vector<int>& dfsnum, std::vector<int>& low, lptlist& SRGlow, std::vector<bool>& SRG, int cur, int parent)
{
	dfsnum[v] = cur;
	cur++;
	int degree = 0;
	low[v] = dfsnum[v];
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
}

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
            if(opp->SRGID != j->SRGID)
            {
                err << "SRGID mismatch for edge " << i << "," << j->dest << "\n";
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
                err << "SRG with contradictory active states: " << i << "\n";
                return false;
            }
        for(int j=1; j<SRGs[i].edges.size(); ++j)
        {
            if(SRGs[i].edges[j-1]->active != SRGs[i].edges[j]->active)
            {
                err << "SRG with contradictory active states: " << i << "\n";
                return false;
            }
        }
        if(SRGs[i].nodes.size() != 0 && SRGs[i].edges.size() != 0 && nodes[SRGs[i].nodes[0]].active != SRGs[i].edges[0]->active)
        {
            err << "SRG with contradictory active states: " << i << "\n";
            return false;
        }
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
        for(std::list<Edge>::iterator j=nodes[i].edges.begin(); j!=nodes[i].edges.end(); ++j)
        {
            if(j->SRGID != -1)
            {
                Edge* e = &(*j);
                bool found = false;
                for(int k=0; k<SRGs[j->SRGID].edges.size(); ++k)
                    if(e == SRGs[j->SRGID].edges[k])
                        found = true;
                if(!found)
                {
                    err << "Edge in SRG " << nodes[i].SRGID << " not found in SRG edge list\n";
                    return false;
                }
            }
        }
    }

    //Consistency for SRG lists w/ node and edge lists
    for(int i=0; i<SRGnum; ++i)
    {
        for(int j=0; j<SRGs[i].nodes.size(); ++j)
        {
            if(nodes[SRGs[i].nodes[j]].SRGID != i)
            {
                err << "Node in SRG " << nodes[i].SRGID << " has mismatched SRGID in node list\n";
                return false;
            }
        }

        for(int j=0; j<SRGs[i].edges.size(); ++j)
        {
            if(SRGs[i].edges[j]->SRGID != i)
            {
                err << "Edge in SRG " << nodes[i].SRGID << " has mismatched SRGID in edge list\n";
                return false;
            }
        }
    }
    return true;
}
