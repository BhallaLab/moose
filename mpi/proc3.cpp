/**
 * This version uses the iscatter call
 */
#include <mpi.h>
#include <vector>
using namespace std;

#define WORKTAG 1
#define DIETAG 2


/* Local functions */

const int numEntries = 10;
const int totCalls = 2 * 65536;
static vector< vector< double > > recvBuf;
static int clearPending( int numNodes, int myrank, MPI_Request *recvReq,
			   double& tot );

static double* get_next_work_item( int numCalls )
{
	static vector< double > ret( numEntries );
	for ( int i = 0; i < numEntries; ++i )
		ret[i] = i + numCalls;

	if ( numCalls >= totCalls )
		return 0;
	return &ret[0];
}

static double doWork(double* work);

int main(int argc, char **argv)
{
  double tot = 0.0;
  double tc = totCalls;
  double ne = numEntries;
  double expectedTot = 
		tc * ( ( ne * (ne - 1.0) )/2.0 ) + 
		ne * ( tc * (tc - 1.0) )/2.0;
  int myrank;
  int numNodes;

  /* Initialize MPI */

  MPI_Init(&argc, &argv);

  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numNodes);

  double sendBuf[numNodes * numEntries];
  double recvBuf[ numEntries ];

  int numCallsPerNode = totCalls / numNodes;

  int begin = myrank * numCallsPerNode;
  int end = begin + numCallsPerNode;

  int k = 0;
  for ( int i = 0; i < numCallsPerNode; ++i ) {
	double* work = get_next_work_item( i + begin );
	int targetNode = i % numNodes;
	for ( int j = 0; j < numEntries; ++j ) {
		sendBuf[targetNode * numEntries + j ] = work[j];
	}

	if ( targetNode == numNodes - 1 ) {
		for ( int j = 0; j < numNodes; ++j ) {
			MPI_Scatter( sendBuf, numEntries, MPI_DOUBLE,
				recvBuf, numEntries, MPI_DOUBLE, j,
				MPI_COMM_WORLD
			); 
			tot += doWork( recvBuf );
		}
	}
  }
  // Final pass to consolidate all the data
  double totBuf[ numNodes ];
  MPI_Allgather( &tot, 1, MPI_DOUBLE, 
				  totBuf, 1, MPI_DOUBLE, 
				  MPI_COMM_WORLD
			   );
  double subtot = tot;
  tot = 0;
  for ( int i = 0; i < numNodes; ++i )
  {
		 tot += totBuf[i];
  }

  cout << myrank << ": Tot = " << tot << 
		  ", expected = " << expectedTot << 
		  ", subtot = " << subtot << endl;

  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}


static double
doWork(double* work)
{
	double tot = 0;
	for (int i =0; i < numEntries; ++i )
		tot += work[i];
}
