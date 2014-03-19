#include "../include/SRGAnnealing.h"
#include <cmath>

namespace mikeNets{

	bool metropolis(double old, double knew, double T)
	{
		return knew < old || double(rand())/RAND_MAX <= exp((old-knew)/T);
	}

	int SRGAnnealing(SRGGraph & g, std::vector<bool>& relays)
	{
		//setup
		int r(0);
		double eval = g.evaluate(relays);
		double T(100.0), alpha(0.97);
		for(int i=0; i<relays.size(); ++i)
		    if(relays[i])
		        ++r;
		std::vector<bool> best(relays);
		int opt = r;
		double evalOld = eval;

		//get started
		for(int its = 0; its<100  || eval != evalOld; ++its)
		{
		    evalOld = eval;
		    for(int i=0; i<relays.size(); ++i)
		    {
		        if(!relays[i])
		            continue;
		        //drop i
		        relays[i]=false;
		        if(g.biConnected(relays))
		        {
		            eval = g.evaluate(relays);
		            --r;
		            if(r<opt)
		            {
		                std::copy(relays.begin(),relays.end(),best.begin());
		                opt = r;
		            }
		            continue;
		        }
		        if(rand()%2) //add or swap
		            relays[i]=true;
		        //better to produce candidate list and shuffle here!
		        for(int j=0; j<30; ++j)
		        {
		            int chosen;
		            do
		            {
		                chosen = rand()%relays.size();
		            }
		            while(relays[chosen] || chosen == i);

		            relays[chosen]=true;
		            if(g.biConnected(relays) && metropolis(eval,g.evaluate(relays),T))
		            {
		                eval = g.evaluate(relays); //change evaluation, keep vectors
		                if(relays[i])
		                    ++r;
		                break;
		            }
		            else
		            {
		                relays[chosen]=false; //move rejected, keep evaluation
		                relays[i]=true;
		            }
		        }
		    }
		    T *= alpha;
		}
		std::copy(best.begin(),best.end(),relays.begin());
		return opt;
	}

}
