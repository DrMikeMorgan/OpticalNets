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
        Node():SRGID(-1),active(true){}
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
    std::vector<Node> nodes;
    std::vector<SingleRiskGroup> SRGs;
    std::size_t m;
    std::vector<float> coords;
    void DFS(int node, std::vector<bool>& marks);
    bool LPTRec(int v, std::vector<int>& dfsnum, std::vector<int>& low, int cur, int parent);
    bool LPTSRG(int v, std::vector<int>& dfsnum, std::vector<int>& low, lptlist& SRGlow, std::vector<bool>& SRG, int cur, int parent);
public:
    SRGGraph():m(0){}
    SRGGraph(int size):nodes(size),m(0),coords(size*2){}
    Node& operator [] (size_t index) {return nodes[index];} //is this actually needed or just a potential risk?
    std::size_t size(){return nodes.size();}
    std::size_t edgeCount(){return m;}
    void AddEdge(int i, int j, int SRG=-1);
    void AddToSRG(int node, int SRG){nodes[node].SRGID=SRG; SRGs[SRG].nodes.push_back(node);}
    void AddToSRG(int i, int j, int SRG);
    int AddSRG(){SRGs.push_back(SingleRiskGroup()); return SRGs.size()-1;}
    friend std::ostream& operator << (std::ostream& o, SRGGraph g);
    void disableSRG(int SRG);
    void enableSRG(int SRG);
    bool connected();
    bool biConnected();
    bool biConFast();
    void clear();
    void setCoords(std::vector<float>& cds){std::copy(cds.begin(),cds.end(),coords.begin());}
    std::vector<float>& getCoords(){return coords;}
    //check class invariants and output if there is a problem
    bool checkInvariant(std::ostream& o);
};

#endif // SRGGRAPH_H
