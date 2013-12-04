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
        double getDistance(std::size_t i, std::size_t j);
        bool checkDistance(std::size_t i, std::size_t j, double distance);
    protected:
    private:
        double Dijkstra(int src, int dest, int disabled = -1);
        void FloydWarshall(size_t disabled = std::numeric_limits<size_t>::max());
		void ModDijkstra(std::size_t src, std::list<size_t>& lightpaths, std::vector<std::list<std::size_t> >& SRGs);
		std::vector<std::list<Edge> > nodes; //need to activate/deactivate nodes
        std::vector<std::vector<double> > matrix;
        double m_max;
        size_t m;
        std::vector<float> coords;
};




#endif // OPTNET_H
