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
        size_t src;
        size_t dest;
        bool active;
        size_t risk;
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
    void init();
public:
    SRGGraph():m(0),n(0),SRGnum(0){}
    SRGGraph(std::size_t sz):m(0),n(sz),SRGnum(0){init();}
    ~SRGGraph(){delete [] nodes; delete [] coords;delete [] SRGs;}
    Node& operator [] (size_t index) {return nodes[index];} //is this actually needed or just a potential risk?
    std::size_t size(){return n;}
    std::size_t edgeCount(){return m;}
    void AddEdge(int i, int j, int SRG=-1);
    void AddToSRG(int node, int SRG){nodes[node].SRGID=SRG; SRGs[SRG].nodes.push_back(node);}
    void AddToSRG(int i, int j, int SRG);
    int AddSRG(){return SRGnum++;}
    SingleRiskGroup& getSRG(std::size_t i){return SRGs[i];}
    friend std::ostream& operator << (std::ostream& o, SRGGraph g);
    void disableSRG(int SRG);
    void enableSRG(int SRG);
    bool connected();
    bool biConnected();
    bool biConnected(std::vector<bool>& relays);
    void clear();
    void setCoords(std::vector<float>& cds){std::copy(cds.begin(),cds.end(),coords);}
    float* getCoords(){return coords;}
    double evaluate(std::vector<bool> & relays);
    bool checkInvariant(std::ostream& o);
};

#endif // SRGGRAPH_H
