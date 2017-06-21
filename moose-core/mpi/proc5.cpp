#include <mpi.h>
#include <vector>
using namespace std;

#define WORKTAG 1
#define DIETAG 2

// Tests out the use of much larger recv buffers than send
// sizes.

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

  MPI_Request recvReq[ numNodes ];
  MPI_Request sendReq[ numNodes ];
  for ( int i = 0; i < numNodes; ++i )
		 recvReq[i] = sendReq[i] = 0;
  vector< double > temp( numEntries * numNodes, 0.0 );
  recvBuf.resize( numNodes, temp );

  int numCallsPerNode = totCalls / numNodes;

  int begin = myrank * numCallsPerNode;
  int end = begin + numCallsPerNode;

  int k = 0;
  for ( int i = 0; i < numNodes; ++i ) {
	if ( i != myrank ) {
    	MPI_Irecv(
			&(recvBuf[i][0]),           /* message buffer */
             numEntries * (1 + i ),                 /* buffer size */
             MPI_DOUBLE,        /* of type double real */
             i,    				/* receive from specified node */
             MPI_ANY_TAG,       /* any type of message */
             MPI_COMM_WORLD,    /* default communicator */
             &recvReq[k++]); 	 /* info about the received message */
	}
  }

  for ( int i = 0; i < numCallsPerNode; ++i ) {
	double* work = get_next_work_item( i + begin );
	int targetNode = i % numNodes;
	if ( targetNode == myrank ) {
		tot += doWork( work );
	} else { // Ship it out to work; and handle shipments that come in.
			 /*
    	MPI_Send(work,             // message buffer 
             numEntries,                 // one data item
             MPI_DOUBLE,           // data item is an integer
             targetNode, 		// Where to send to
             WORKTAG,           // user chosen message tag
             MPI_COMM_WORLD   // default communicator
		);
		*/
    	MPI_Isend(work,             // message buffer 
             numEntries,                 // one data item
             MPI_DOUBLE,           // data item is an integer
             targetNode, 		// Where to send to
             WORKTAG,           // user chosen message tag
             MPI_COMM_WORLD,   // default communicator
			 &sendReq[targetNode] // Info about outgoing message
		);
	}
	if ( targetNode == numNodes - 1 ) {
		int numDone = 1;
		while ( numDone < numNodes ) // Ensure we clear all once a cycle
			numDone += clearPending( numNodes, myrank, recvReq, tot ); 
	}
  }
  // One last send with the consolidated result. Irecvs should have
  // been posted already.
  	vector< double > work( numEntries, 0.0 );
	work[0] = tot;
  for ( int i = 0; i < numNodes; ++i ) {
		 if ( i == myrank ) continue;
    	MPI_Send(&work[0],             // message buffer 
             numEntries,                 // one data item
             MPI_DOUBLE,           // data item is an integer
             i, 		// Where to send to
             WORKTAG,           // user chosen message tag
             MPI_COMM_WORLD   // default communicator
		);
	}

	int numDone = 1;
	while ( numDone < numNodes ) // Ensure we clear all once a cycle
			numDone += clearPending( numNodes, myrank, recvReq, tot ); 

  cout << myrank << ": Tot = " << tot << 
		  ", expected = " << expectedTot << 
		  ", subtot = " << work[0] << endl;

  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}

int clearPending( int numNodes, int myrank, MPI_Request *recvReq, double& tot )
{
	if ( numNodes == 1 )
		return 0;
	int doneIndices[ numNodes ];
	int done = 0;
	MPI_Status doneStatus[ numNodes ];
	for ( int i = 0; i < numNodes; ++i ) {
		doneIndices[i] = 0;
		MPI_Status &ds = doneStatus[i];
		ds.MPI_SOURCE = ds.MPI_TAG = ds.MPI_ERROR = ds._count = ds._cancelled = 0;
	}

	int numDone = MPI_Testsome( numNodes - 1, recvReq, &done, 
					doneIndices, doneStatus );
	// cout << "numDone = " << numDone << ", " << done << ", numNodes = " << numNodes << ", myrank = " << myrank << endl << flush;
	if ( done == MPI_UNDEFINED )
		return 0;
	for ( unsigned int i = 0; i < done; ++i ) {
		int recvNode = doneIndices[i];
		if ( recvNode >= myrank )
				recvNode += 1; // Skip myrank
		double* work = &(recvBuf[recvNode][0]);
		tot += doWork( work );
		// Post the Recv again.
    	MPI_Irecv(
			 work,           /* message buffer */
             numEntries * (1 + myrank),                 /* buffer size */
             MPI_DOUBLE,        /* of type double real */
             recvNode,    /* receive from specified node */
             MPI_ANY_TAG,       /* any type of message */
             MPI_COMM_WORLD,    /* default communicator */
             &recvReq[doneIndices[i]]);  /* info about the received message */
	}
	return done; 
}


static double
doWork(double* work)
{
	double tot = 0;
	for (int i =0; i < numEntries; ++i )
		tot += work[i];
}
