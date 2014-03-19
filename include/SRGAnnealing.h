#ifndef SRGANNEALING_H
#define SRGANNEALING_H

#include <vector>

#include "SRGGraph.h"

namespace mikeNets
{

int SRGAnnealing(SRGGraph & g, std::vector<bool>& relays);

}
#endif // SRGANNEALING_H
