#ifndef SRGGRAPH_H
#define SRGGRAPH_H
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

typedef std::list<std::pair<int,std::vector<int> > > lptlist;

class SRGGraph
{
public:
    struct Edge
    {
        int dest;
        bool active;
        int SRGID;
    };
    struct Node
    {
        int SRGID;
        bool active;
        std::list<Edge> edges;
    };
    struct SingleRiskGroup
    {
        std::vector<int> nodes;
        std::vector<Edge*> edges;
    };

private:
    Node * nodes;
    SingleRiskGroup * SRGs;
    std::size_t SRGnum;
    std::size_t m;
    std::size_t n;
    float * coords;
    void DFS(int node, std::vector<bool>& marks);
    bool LPTRec(int v, std::vector<int>& dfsnum, std::vector<int>& low, int cur, int parent);
    bool LPTSRG(int v, std::vector<int>& dfsnum, std::vector<int>& low, lptlist& SRGlow, std::vector<bool>& SRG, int cur, int parent);

public:
    SRGGraph():m(0),n(0),SRGnum(0){}
    SRGGraph(std::size_t sz):m(0),n(sz),SRGnum(0){}
    void init()
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
    ~SRGGraph(){delete [] nodes; delete [] coords;delete [] SRGs;}
    Node& operator [] (size_t index) {return nodes[index];} //is this actually needed or just a potential risk?
    std::size_t size(){return n;}
    std::size_t edgeCount(){return m;}
    void AddEdge(int i, int j, int SRG=-1);
    void AddToSRG(int node, int SRG){nodes[node].SRGID=SRG; SRGs[SRG].nodes.push_back(node);}
    void AddToSRG(int i, int j, int SRG);
    int AddSRG(){return SRGnum++;}
    friend std::ostream& operator << (std::ostream& o, SRGGraph g);
    void disableSRG(int SRG);
    void enableSRG(int SRG);
    bool connected();
    bool biConnected();
    bool biConFast();
    void clear();
    void setCoords(std::vector<float>& cds){std::copy(cds.begin(),cds.end(),coords);}
    float* getCoords(){return coords;}
    //check class invariants and output if there is a problem
    bool checkInvariant(std::ostream& o);
};

#endif // SRGGRAPH_H
