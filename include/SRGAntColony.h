#ifndef SRGANTCOLONY_H
#define SRGANTCOLONY_H

#include <vector>
#include "SRGGraph.h"

class PheromoneMatrix : public std::vector<std::vector<float> >
{
    public:
        PheromoneMatrix(std::size_t n);
        void update(std::vector<std::size_t>& nodes, float result);
        void setEvaporationRate(float rate){rho = rate;}
        void setQ(float q){Q = q;}
        float init(std::size_t i){return firstChoice[i];}
    protected:
        float rho; //evaporation rate
        float Q; //base constant
        std::vector<float> firstChoice;
    private:
};

int SRGAntColony(SRGGraph& g, std::vector<bool>& relays, std::size_t genSize, std::size_t iterations);

#endif // SRGANTCOLONY_H
