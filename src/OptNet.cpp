#include "../include/OptNet.h"
#include <iostream>
#include <fstream>

template<class T>
class DijkHeap
{
    std::vector<std::pair<std::size_t,T> > data; //data is a heap of pairs (node, tentative weight) for Dijkstra
    std::vector<std::size_t> heapLocs; //stores index of current location on heap
    int n;
    void swap(int i, int j);
   public:
    DijkHeap(std::size_t maxSize):n(0),data(maxSize),heapLocs(maxSize,std::numeric_limits<std::size_t>::max()){}
    void insert(std::size_t id, T item);
    std::size_t extractMin();
    std::size_t extractMin(T& value);
    void decreaseKey(std::size_t idx, T newKey);
    bool empty(){return n==0;}
};

template <class T>
void DijkHeap<T>::swap(int i, int j)
{
    std::pair<std::size_t,T> temp = data[i];
    data[i] = data[j];
    data[j] = temp;
    heapLocs[data[j].first] = j;
    heapLocs[data[i].first] = i;
    //std::cout << "swapping...\n";
}

template <class T>
void DijkHeap<T>::insert(std::size_t id, T item)
{
    int cur = n;
    data[n] = std::make_pair(id,item);
    heapLocs[id] = cur;
    ++n;
    //std::cout << "Comparing " << data[cur].second << " with " << data[(cur-1)/2].second << "\n";
    //std::cout << "Cur = " << cur<< "\n";
    while(cur > 0 && data[cur].second < data[(cur-1)/2].second)
    {

        this->swap(cur,(cur-1)/2);
        cur = (cur-1)/2;
    }
}

template <class T>
void DijkHeap<T>::decreaseKey(std::size_t idx, T newKey)
{
    if(newKey >= data[heapLocs[idx]].second)
        return;
    std::size_t cur  = heapLocs[idx];
    data[heapLocs[idx]].second = newKey;
    while(cur > 0 && data[cur].second < data[(cur-1)/2].second)
    {
        this->swap(cur,(cur-1)/2);
        cur = (cur-1)/2;
    }
}

template <class T>
std::size_t DijkHeap<T>::extractMin()
{
    std::size_t ret = data[0].first;
    int cur = 0;
    --n;
    swap(0,n);
    heapLocs[ret] = std::numeric_limits<std::size_t>::max();
    while((2*cur+1 < n && data[cur].second > data[2*cur+1].second) || (2*cur+2<n && data[cur].second >  data[2*cur+2].second))
    {
        std::size_t swapper;
        if(n == 2*cur+2)
            swapper = 2*cur+1;
        else
            swapper = data[2*cur+1].second < data[2*cur+2].second ? 2*cur+1 : 2*cur+2;
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
    std::ofstream dbg;
    dbg.open("debug.txt",std::ios::out);
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
             //some heuristic based on length of recompute set?
            //FloydWarshall(i);
            for (std::list<std::pair<int,int> >::iterator j = recomputeSet.begin(); j!=recomputeSet.end(); ++j)
                if(j->first != i && j->second != i && Dijkstra(j->first,j->second,i) > m_max) //matrix[j->first][j->second] > m_max)
                {
                    SRGs[i].push_back(std::make_pair(j->first,j->second));
                }

        }
    }
    dbg << recomputedPaths;
    dbg.close();

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

OptNet::OptNet(int size, double mtd): nodes(size),m_max(mtd),m(0),coords(size*2),matrix(size,std::vector<double>(size, std::numeric_limits<double>::max())),contains(size,std::vector<std::vector<bool> >(size,std::vector<bool>(size,false))), maxloss(size,0), minloss(size,0)
{
    for(int i=0; i<size; ++i)
    {
        coords[i*2] = double(rand())/RAND_MAX;
        coords[i*2+1] = double(rand())/RAND_MAX;
    }

}
