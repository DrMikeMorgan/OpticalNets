#ifndef OPTNET_H
#define OPTNET_H

#include <vector>
#include <limits>
#include "SRGGraph.h"


class OptNet
{
    public:
        struct Edge
        {
            int dest;
            double length;
        };
        OptNet():m(0){}
        OptNet(int size, double mtd);
        //virtual ~OptNet();
        void AddEdge(int u, int v, double length);
        void GetSRGs(SRGGraph& g);
        size_t edgeCount(){return m;}
        std::list<Edge>& operator [] (size_t i) {return nodes[i];}
        std::vector<float>& getCoords() {return coords;}
        size_t size(){return nodes.size();}
    protected:
    private:
    double Dijkstra(int src, int dest, int disabled = -1);
    void FloydWarshall(size_t disabled = std::numeric_limits<size_t>::max());
    std::vector<std::list<Edge> > nodes; //need to activate/deactivate nodes
    std::vector<std::vector<double> > matrix;
    std::vector<double> maxloss;
    std::vector<double> minloss;
    std::vector<std::vector<std::vector<bool> > > contains;
    double m_max;
    size_t m;
    std::vector<float> coords;
};




#endif // OPTNET_H
