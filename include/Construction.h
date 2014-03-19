#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H
#include "SRGGraph.h"

namespace mikeNets
{
std::size_t dropAlgorithm(SRGGraph& g, std::vector<bool>& relays, std::size_t threshold = 0);
}

#endif // CONSTRUCTION_H
