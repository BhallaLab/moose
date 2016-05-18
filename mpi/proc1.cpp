/*
void process( const Eref& e, ProcPtr p )
{
	for ( unsigned int i = 0; i < numNodes; ++i ){
		if ( i != myNode ) {
			isend( outbuf_[i] );

			inbufNum = recv(from anywhere)
			readBuf( inbuf_[inbufNum] );
		}
	}
}

vector< vector< double > > outBuf_; // outbuf[tgtnode][data]
vector< vector< double > > inBuf_;// inbuf[srcnode][data]


// Things to monitor:
// - The send happens with a standard buf size, and if the contents 
// 	exceed it there is a signal to this effect and the rest of it is sent
// 	right away as a big block.
// - The Recv likewise
//
// mpi_testsome checks which msgs have been received.
//
//

// Assumes we already have an irecv out for all the nodes.
// 
void checkIncoming()
{
	MPI_request* array_of_requests;
	MPI_status* array_of_statuses;

		MPI_testsome()
	int MPI_Testsome(int incount, MPI_Request array_of_requests[],
	int *outcount, int array_of_indices[], MPI_Status array_of_statuses[])

	for ( unsigned int 

	MPI_Irecv (&buf,count,datatype,source,tag,comm,&request) 
}


main()
{
}
*/

#include <mpi.h>
#include <vector>
using namespace std;

#define WORKTAG 1
#define DIETAG 2


/* Local functions */

const int numEntries = 10;
const int totCalls = 1000;
static void master(void);
static void slave(void);
static double* get_next_work_item()
{
	
	static vector< double > ret( numEntries );
	static unsigned int numCalls = 0;
	for ( unsigned int i = 0; i < numEntries; ++i )
		ret[i] = i + numCalls;

	numCalls++;
	if ( numCalls > totCalls )
		return 0;
	return &ret[0];
}

static double do_work(double* work);

int main(int argc, char **argv)
{
  int myrank;

  /* Initialize MPI */

  MPI_Init(&argc, &argv);

  /* Find out my identity in the default communicator */

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  if (myrank == 0) {
    master();
  } else {
    slave();
  }

  /* Shut down MPI */

  MPI_Finalize();
  return 0;
}



static void
master(void)
{
  int ntasks, rank;
  double* work;
  double result;
  MPI_Status status;
  double tot = 0.0;
  double tc = totCalls;
  double ne = numEntries;
  double expectedTot = 
		tc * ( ( ne * (ne - 1.0) )/2.0 ) + 
		ne * ( tc * (tc - 1.0) )/2.0;

  /* Find out how many processes there are in the default
     communicator */

  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  /* Seed the slaves; send one unit of work to each slave. */

  for (rank = 1; rank < ntasks; ++rank) {

    /* Find the next item of work to do */

    work = get_next_work_item();

    /* Send it to each rank */

    MPI_Send(work,             /* message buffer */
             numEntries,                 /* one data item */
             MPI_DOUBLE,        /* data item is a double */
             rank,              /* destination process rank */
             WORKTAG,           /* user chosen message tag */
             MPI_COMM_WORLD);   /* default communicator */
  }

  /* Loop over getting new work requests until there is no more work
     to be done */

  work = get_next_work_item();
  while (work != NULL) {

    /* Receive results from a slave */

    MPI_Recv(&result,           /* message buffer */
             1,                 /* one data item */
             MPI_DOUBLE,        /* of type double real */
             MPI_ANY_SOURCE,    /* receive from any sender */
             MPI_ANY_TAG,       /* any type of message */
             MPI_COMM_WORLD,    /* default communicator */
             &status);          /* info about the received message */

    /* Send the slave a new work unit */

    MPI_Send(work,             /* message buffer */
             numEntries,                 /* one data item */
             MPI_DOUBLE,           /* data item is an integer */
             status.MPI_SOURCE, /* to who we just received from */
             WORKTAG,           /* user chosen message tag */
             MPI_COMM_WORLD);   /* default communicator */

    /* Get the next unit of work to be done */

    work = get_next_work_item();
	tot += result;
  }

  /* There's no more work to be done, so receive all the outstanding
     results from the slaves. */

  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	tot += result;
  }

  /* Tell all the slaves to exit by sending an empty message with the
     DIETAG. */

  for (rank = 1; rank < ntasks; ++rank) {
    MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
  }
  cout << "Tot = " << tot << ", expected = " << expectedTot << endl;
}


static void 
slave(void)
{
  double work[numEntries];
  double result;
  MPI_Status status;

  while (1) {

    /* Receive a message from the master */

    MPI_Recv( work, numEntries, MPI_DOUBLE, 0, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);

    /* Check the tag of the received message. */

    if (status.MPI_TAG == DIETAG) {
      return;
    }

    /* Do the work */

    result = do_work(work);

    /* Send the result back */

    MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
}

static double
do_work(double* work)
{
	double tot = 0;
	for (int i =0; i < numEntries; ++i )
		tot += work[i];
}
