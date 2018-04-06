//// Fast Network Simplex for optimal mass transport -- test unit
//// by Nicolas Bonneel (Nov. 2013)
///
//// Typical runtime: 10k source nodes + 10k destination nodes :
///  -  double precision: 254 seconds using 2.5GB of RAM
///  -  single precision: 118 seconds using 1.7GB of RAM
///  -  int : 89 seconds using 1.7 GB of RAM
///  -  __int64 : 95 seconds using 2.5 GB of RAM


#include <iostream>
#include <vector>

#include "network_simplex_simple.h"
#include "chrono.h"

using namespace lemon;

typedef unsigned int node_id_type; // id of nodes ; integral type ; Should be able to handle (nb of arcs + number of nodes*2) (UINT_MAX = 4294967295 = 65535^2) ; could be __int64 or long long (may need to adapt the code if using more than that)
typedef float supply_type; // {float, double, int, or potentially __int64/long long}. Should be able to handle the sum of supplies and *should be signed* (a demand is a negative supply)
typedef float cost_type;  // {float, double, int or __int64/long long} ; Should be able to handle (number of arcs * maximum cost) and *should be signed* 

struct TsFlow {
	node_id_type from, to;
	supply_type amount;
};

cost_type sqrdistance2d(double* a, double* b) {
	return (a[0]-b[0])*(a[0]-b[0])+(a[1]-b[1])*(a[1]-b[1]);
}

int main() {

		typedef FullBipartiteDigraph Digraph;
		DIGRAPH_TYPEDEFS(FullBipartiteDigraph);

		node_id_type n1 = 10000, n2=n1; // just demo for the case n1=n2 ; adapt otherwise
		std::vector<double> coordsXY(n1*2);
		std::vector<supply_type> weights1(n1), weights2(n2); // works faster with integer weights though

		Digraph di(n1, n2);
		NetworkSimplexSimple<Digraph,supply_type,cost_type, node_id_type> net(di, true, n1+n2, n1*n2);

		// random point coordinates in 2D ; the same for both distributions here
		for (node_id_type i=0; i<n1*2; i++) {
			coordsXY[i] = rand()*100./(double)RAND_MAX;
		}

		node_id_type idarc = 0;
		for (node_id_type i=0; i<n1; i++) {
			for (node_id_type j=0; j<n2; j++) {
				cost_type d = sqrdistance2d(&coordsXY[i*2], &coordsXY[j*2]);
				net.setCost(di.arcFromId(idarc), d); 
				idarc++;
			}
		}		

		// random supplies and demands
		for (node_id_type i=0; i<n1; i++) {
			weights1[ di.nodeFromId(i) ] = rand()%10+1;
		}
		for (node_id_type i=0; i<n2; i++) {
			weights2[ di.nodeFromId(i) ] = -(rand()%10+2); 
			// +2 to artifically make sure (or rather, more likely) that there is more demand than supplies, for testing purpose. 
			// Negative sign means a demand (there should always be negative values in the target histogram)
		}
		
		net.supplyMap(&weights1[0], n1, &weights2[0], n2);

		PerfChrono chrono;
		chrono.Start();

		int ret = net.run();

		double resultdist = net.totalCost();
		std::cout<<(double)chrono.GetDiffMs()/1000.<<" s"<<std::endl;
		
		std::vector<TsFlow> flow;
		flow.reserve(n1+n2-1);

		for (node_id_type i=0; i<n1; i++) {
			for (node_id_type j=0; j<n2; j++)
			{
				TsFlow f;
				f.amount = net.flow(di.arcFromId(i*n2+j));
				f.from = i;
				f.to = j;				
				if (fabs((double)f.amount)>1E-18)  // if everything went well, all these flows are actually integers
					flow.push_back(f);
			}
		}

		std::cout<<resultdist<<std::endl;
		Sleep(100000000);

		return 0;
}