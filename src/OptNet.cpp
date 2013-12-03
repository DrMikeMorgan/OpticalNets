#include "../include/OptNet.h"
#include <iostream>
#include <fstream>
#include <cmath>

#define parent(i) (i-1)/2
#define lchild(i) (2*i+1)
#define rchild(i) (2*i+2)

template<class T>
class DijkHeap
{
    std::vector<std::pair<std::size_t,T> > data; //data is a heap of pairs (node, tentative weight) for Dijkstra
    std::vector<std::size_t> heapLocs; //stores index of current location on heap
    int n;
    void swap(size_t i, size_t j);
   public:
    DijkHeap(std::size_t maxSize):n(0),data(maxSize),heapLocs(maxSize,std::numeric_limits<std::size_t>::max()){}
    void insert(std::size_t id, T item);
    std::size_t extractMin();
    std::size_t extractMin(T& value);
    void decreaseKey(std::size_t idx, T newKey);
    bool empty(){return n==0;}
    int checkInvariant();
};

template<class T>
int DijkHeap<T>::checkInvariant()
{
    for(int i=1; i<n; ++i)
    {
        if(data[i].second < data[parent(i)].second)
            return 1;
        if(heapLocs[data[i].first] == std::numeric_limits<std::size_t>::max())
            return 2;
        if(data[i].first != data[heapLocs[data[i].first]].first)
            return 3;
    }
    return 0;
}

template <class T>
void DijkHeap<T>::swap(size_t i, size_t j)
{
    std::pair<std::size_t,T> temp = data[i];
    data[i] = data[j];
    data[j] = temp;
    heapLocs[data[j].first] = j;
    heapLocs[data[i].first] = i;
}

template <class T>
void DijkHeap<T>::insert(std::size_t id, T item)
{
    int cur = n;
    data[n] = std::make_pair(id,item);
    heapLocs[id] = cur;
    ++n;
    while(cur > 0 && data[cur].second < data[parent(cur)].second)
    {

        this->swap(cur,parent(cur));
        cur = parent(cur);
    }
}

template <class T>
void DijkHeap<T>::decreaseKey(std::size_t idx, T newKey)
{
    if(newKey >= data[heapLocs[idx]].second) //bounce if not decreased
        return;
    std::size_t cur  = heapLocs[idx];
    if(cur == std::numeric_limits<size_t>::max()) //bounce if idx not found in heap
        return;
    data[heapLocs[idx]].second = newKey;
    while(cur > 0 && data[cur].second < data[parent(cur)].second)
    {
        this->swap(cur,parent(cur));
        cur = parent(cur);
    }
}

template <class T>
std::size_t DijkHeap<T>::extractMin()
{
    std::size_t ret = data[0].first;
    size_t cur = 0;
    swap(0,n-1);
    --n;
    heapLocs[ret] = std::numeric_limits<std::size_t>::max();
    while((lchild(cur) < n && data[cur].second > data[lchild(cur)].second) || (rchild(cur)<n && data[cur].second >  data[rchild(cur)].second))
    {
        std::size_t swapper;
        if(n == rchild(cur))
            swapper = lchild(cur);
        else
            swapper = data[lchild(cur)].second < data[rchild(cur)].second ? lchild(cur) : rchild(cur);
        this->swap(cur,swapper);
        cur = swapper;
    }

    return ret;
}

template <class T>
std::size_t DijkHeap<T>::extractMin(T& value)
{
    value = data[0].second;
    return extractMin();
}

double OptNet::Dijkstra(int src, int dest, int disabled)
{
    DijkHeap<double> heap(nodes.size());
    std::vector<bool> in(nodes.size(),false);
    heap.insert(src,0);
    in[src]=true;
    while(!heap.empty())
    {
        double weight;
        int cur = heap.extractMin(weight);
        if(cur == disabled)
            continue;
        if(cur == dest)
            return weight;
        for(std::list<Edge>::iterator j = nodes[cur].begin(); j != nodes[cur].end(); ++j)
        {
            //std::cout << "edge to " << j->dest <<"(";
            double newval = weight + j->length;
            if(!in[j->dest])
            {
                heap.insert(j->dest,newval);
                in[j->dest] = true;
            }
            else
                heap.decreaseKey(j->dest,newval);
        }
    }
    return std::numeric_limits<double>::max(); // no path from src to dest
}


void OptNet::AddEdge(int u, int v, double length)
{
    nodes[u].push_back({v,length});
    nodes[v].push_back({u,length});
    ++m;
}

void OptNet::FloydWarshall(size_t disabled)
{
    size_t n = nodes.size();
    std::vector<std::vector<size_t> > successor(n, std::vector<size_t>(n,n));
    for(int i=0; i<n; ++i)
    {
        for(int j=i+1; j<n; ++j)
            matrix[i][j] = matrix[j][i] = std::numeric_limits<double>::max();
        matrix[i][i] = 0;
        if(i == disabled)
            continue;


        if(disabled > n)
        {
            maxloss[i]=minloss[i]=0;
            for(int j=0; j<n; ++j)
            {
                successor[i][j] = n;
                for(int k=0; k<n; ++k)
                    contains[i][j][k] = false;
            }
        }

        for(std::list<Edge>::iterator j = nodes[i].begin(); j!=nodes[i].end(); ++j)
            if(j->dest != disabled)
            {
                matrix[i][j->dest] = j->length;
                successor[i][j->dest] = j->dest;
            }
    }
    for(int k=0; k<n; ++k)
        for(int i=0; i<n; ++i)
            for(int j=0; j<n; ++j)
            {
                double diff = matrix[i][j] - (matrix[i][k] + matrix[k][j]);
                if(diff > 0)
                {
                    matrix[i][j] = matrix[i][k] + matrix[k][j];
                    if(disabled > n)
                    {
                        successor[i][j] = successor[i][k];
                        maxloss[k] = diff > maxloss[k] ? diff : maxloss[k];
                        //minloss[k] = diff < minloss[k] ? diff : minloss[k];
                    }
                }
            }
    for(int i=0; i<n; ++i)
        for(int j=0; j<n; ++j)
        {
            if(successor[i][j]==n)
                continue;
            for(int k=i; k!=j; k=successor[k][j])
                contains[i][j][k]=true;
        }
}

void OptNet::GetSRGs(SRGGraph& g)
{
    std::list<std::pair<int,int> > TLS;
    g.setCoords(coords);

    /* DIJKSTRA VERSION
    for(int i=0; i<nodes.size(); ++i)
        for(int j=i+1; j<nodes.size(); ++j)
            if(Dijkstra(i,j) <= m_max)
            {
                TLS.push_back(std::make_pair(i,j));
            }*/

    FloydWarshall();
    for(int i=0; i<nodes.size(); ++i)
        for(int j=i+1; j<nodes.size(); ++j)
            if(matrix[i][j] <= m_max)
            {
                TLS.push_back(std::make_pair(i,j));
            }


    std::vector<std::list<std::pair<int,int> > > SRGs(nodes.size());

    /* DIJKSTRA VERSION
    for(int i=0; i<nodes.size(); ++i)
        for (std::list<std::pair<int,int> >::iterator j = TLS.begin(); j!=TLS.end(); ++j)
            if(j->first != i && j->second != i && Dijkstra(j->first,j->second,i) > m_max)
            {
                SRGs[i].push_back(std::make_pair(j->first,j->second));
            }
    */


    size_t recomputedPaths = 0;
    for(int i=0; i<nodes.size(); ++i) //O(n^4) !!!!
    {
        bool recompute = false;
        std::list<std::pair<int,int> > recomputeSet;
        for (std::list<std::pair<int,int> >::iterator j = TLS.begin(); j!=TLS.end(); ++j)
            if(contains[j->first][j->second][i] && matrix[j->first][j->second] + maxloss[i] > m_max)
            {
                    recompute = true;
                    recomputeSet.push_back(*j);
                    ++recomputedPaths;
            }

        if(recompute)
        {
            //FloydWarshall(i);
            for (std::list<std::pair<int,int> >::iterator j = recomputeSet.begin(); j!=recomputeSet.end(); ++j)
            {
                if(j->first != i && j->second != i)  //matrix[j->first][j->second] > m_max) //
                {
                    double dist = Dijkstra(j->first,j->second,i);
                    if(dist > m_max)
                        SRGs[i].push_back(std::make_pair(j->first,j->second));
                }

            }
        }

    }


    std::vector<std::vector<bool> > mat(nodes.size(),std::vector<bool>(nodes.size(),false)); //all subsequent appearances of the word 'matrix' should change to refer to this (debug check)
    for(int i=0; i<nodes.size(); ++i)
    {
        for(std::list<Edge>::iterator j = nodes[i].begin(); j != nodes[i].end(); ++j)
        {
            if(i<j->dest)
                g.AddEdge(i,j->dest);
            mat[i][j->dest] = matrix[j->dest][i] = true;
        }
        g.AddToSRG(i,g.AddSRG());
    }


    //this should still work, amazingly - but the invariant check and data structures should be changed
    for(int i=0; i<nodes.size(); ++i)
        for(std::list<std::pair<int,int> >::iterator j=SRGs[i].begin(); j!= SRGs[i].end(); ++j)
            if(!mat[j->first][j->second])
            {
                g.AddEdge(j->first,j->second,i);
                mat[j->first][j->second] = mat[j->second][j->first] = true;
            }
            else
                g.AddToSRG(j->first,j->second,i);

    for(std::list<std::pair<int,int> >::iterator j=TLS.begin(); j!=TLS.end(); ++j)
        if(!mat[j->first][j->second])
            g.AddEdge(j->first,j->second);
}

double OptNet::getDistance(std::size_t i, std::size_t j)
{
    double dx2 = (coords[i*2] - coords[j*2])*(coords[i*2] - coords[j*2]);
    double dy2 = (coords[i*2+1] - coords[j*2+1])*(coords[i*2+1] - coords[j*2+1]);
    return sqrt(dx2 + dy2);
}

bool OptNet::checkDistance(std::size_t i, std::size_t j, double distance)
{
    double dx2 = (coords[i*2] - coords[j*2])*(coords[i*2] - coords[j*2]);
    double dy2 = (coords[i*2+1] - coords[j*2+1])*(coords[i*2+1] - coords[j*2+1]);
    return dx2+dy2 < (distance * distance);
}

OptNet::OptNet(int size, double mtd): nodes(size),m_max(mtd),m(0),coords(size*2),matrix(size,std::vector<double>(size, std::numeric_limits<double>::max())),contains(size,std::vector<std::vector<bool> >(size,std::vector<bool>(size,false))), maxloss(size,0), minloss(size,0)
{
    for(int i=0; i<size; ++i)
    {
        coords[i*2] = double(rand())/RAND_MAX;
        coords[i*2+1] = double(rand())/RAND_MAX;
    }

}
