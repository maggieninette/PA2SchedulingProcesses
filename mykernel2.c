/* mykernel2.c: your portion of the kernel
 *
 *	Below are procedures that are called by other parts of the kernel. 
 *	Your ability to modify the kernel is via these procedures.  You may
 *	modify the bodies of these procedures any way you wish (however,
 *	you cannot change the interfaces).  
 * 
 */

#include "aux.h"
#include "sys.h"
#include "mykernel2.h"

#define TIMERINTERVAL 1000	// in ticks (tick = 10 msec)

/*	A sample process table. You may change this any way you wish.  
 */

static struct {
	int valid;		// is this entry valid: 1 = yes, 0 = no
	int pid;		// process ID (as provided by kernel)
	int timestamp; 
	int passvalue;
	int stridevalue;
	int request;

	int requested; 		//a flag for keepin track of which process has made a request

} proctab[MAXPROCS];



//global variable to keep track of timestamp set to each process.
int globalclock;
int run_next = -1;
int allocated;
int totalprocs;	      //total processes in the system
int requestedprocs;  //number of processes that made requests
int L = 100000;
int last_ran = -1;

/*	InitSched () is called when kernel starts up.  First, set the
 *	scheduling policy (see sys.h). Make sure you follow the rules
 *	below on where and how to set it.  Next, initialize all your data
 *	structures (such as the process table).  Finally, set the timer
 *	to interrupt after a specified number of ticks. 
 */

void InitSched ()
{
	int i;

	/* First, set the scheduling policy.  You should only set it
	 * from within this conditional statement.  While you are working
	 * on this assignment, GetSchedPolicy will return NOSCHEDPOLICY,
	 * and so the condition will be true and you may set the scheduling
	 * policy to whatever you choose (i.e., you may replace ARBITRARY). 
	 * After the assignment is over, during the testing phase, we will
	 * have GetSchedPolicy return the policy we wish to test, and so
	 * the condition will be false and SetSchedPolicy will not be
	 * called, thus leaving the policy to whatever we chose to test.  
	 */
	if (GetSchedPolicy () == NOSCHEDPOLICY) {	// leave as is
		SetSchedPolicy (LIFO);		// set policy here
	}
		
	/* Initialize all your data structures here */
	for (i = 0; i < MAXPROCS; i++) {
		proctab[i].valid = 0;
		proctab[i].passvalue = 0;
		proctab[i].request = 0;
	}

	/* Set the timer last */
	SetTimer (TIMERINTERVAL);
}


/*	StartingProc (pid) is called by the kernel when the process
 *	identified by pid is starting.  This allows you to record the
 *	arrival of a new process in the process table, and allocate
 *	any resources (if necessary). Returns 1 if successful, 0 otherwise.  
 */

int StartingProc (pid)
	int pid;			// process that is starting
{
	int i;

	for (i = 0; i < MAXPROCS; i++) {
		if (! proctab[i].valid) {
			proctab[i].valid = 1;
			proctab[i].pid = pid;
			proctab[i].passvalue = 0;
			proctab[i].timestamp = globalclock;
			globalclock++;
			totalprocs++;

			if (GetSchedPolicy() == LIFO) {
				DoSched();
			}
			if (GetSchedPolicy() == ROUNDROBIN){
				if (run_next == -1) {  //if run_next has not been set yet
					run_next = i;  //save the index
				}
			}
		
			return (1);
		}
	}

	Printf ("Error in StartingProc: no free table entries\n");
	return (0);
}
			

/*	EndingProc (pid) is called by the kernel when the process
 *	identified by pid is ending.  This allows you to update the
 *	process table accordingly, and deallocate any resources (if
 *	necessary). Returns 1 if successful, 0 otherwise.  
 */


int EndingProc (pid)
	int pid;			// process that is ending
{
	int i;

	for (i = 0; i < MAXPROCS; i++) {
		if (proctab[i].valid && proctab[i].pid == pid) {
			proctab[i].valid = 0;
			totalprocs--;

			//give back the allocated time to CPU
			allocated = allocated-proctab[i].request;

			return (1);
		}
	}

	Printf ("Error in EndingProc: can't find process %d\n", pid);
	return (0);
}


/*	SchedProc () is called by kernel when it needs a decision for
 *	which process to run next.  It calls the kernel function
 *	GetSchedPolicy () which will return the current scheduling policy
 *	which was previously set via SetSchedPolicy (policy). SchedProc ()
 *	should return a process id, or 0 if there are no processes to run.  
 */

int SchedProc ()
{
	int i;
	int tmp;
	int temp[MAXPROCS];
	int min_i = -1;
	int max_i = -1;

	switch (GetSchedPolicy ()) {

	case ARBITRARY:

		for (i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) {
				return (proctab[i].pid);
			}
		}
		break;

	case FIFO:

		
                for (i = 0; i < MAXPROCS; i++) {
                        if (proctab[i].valid) {

				//first sets min_process to the first process it sees
				if (min_i == -1) {
					min_i = i; 

				}		

				if (proctab[i].timestamp < proctab[min_i].timestamp) {
					min_i = i;			
				}

                        }
                }	
		return proctab[min_i].pid;

	case LIFO:

		/* your code here */
		
                for (i = 0; i < MAXPROCS; i++) {
                        if (proctab[i].valid) {

				//first sets max_i to the first process it sees
				if (max_i == -1) {
					max_i = i; 

				}		

				if (proctab[i].timestamp > proctab[max_i].timestamp) {
					max_i = i;
			
				}
                        }
                }	
		return proctab[max_i].pid;


	case ROUNDROBIN:

		
		//if last_ran is -1 that means no processes have ran yet, find the first valid process
		if (last_ran == -1) {
			for (int i = 0; i < MAXPROCS; i++) {
				if (proctab[i].valid) {
					last_ran = i;
					return proctab[i].pid;
				}
			}
		}		
		else {  //else you start looking from the index of the last ran process
			for (int i = last_ran+1; i < MAXPROCS; i++) {
				if (proctab[i].valid) {
					last_ran = i;
					return proctab[i].pid;
				}
			}

			//if we've reached the end of the array, start over
			for (int i = 0; i < MAXPROCS; i++) {
				
				if (proctab[i].valid) {
					last_ran = i;
					return proctab[i].pid;
				}
			}
		}
		return (0);


		
		
	case PROPORTIONAL:

		//select process x with minimum pass value
		
                for (i = 0; i < MAXPROCS; i++) {
                        if (proctab[i].valid) {

				//first sets min to the first process it sees
				if (min_i == -1) {
					min_i = i; 

				}		

				if (proctab[i].passvalue < proctab[min_i].passvalue) {
					min_i = i;			
				}

                        }
		}

		int smallest_passvalue = proctab[min_i].passvalue;

		//subtract the pass value of all processes by the smallest pass value
		for (i = 0; i < MAXPROCS; i++) {
			//min_i holds the index of the process with smallest pass value
			if (proctab[i].valid && proctab[min_i].valid) {
				proctab[i].passvalue = proctab[i].passvalue-smallest_passvalue;
				
			}

		}
	
		//adjust the pass value
		proctab[min_i].passvalue = proctab[min_i].passvalue + proctab[min_i].stridevalue;
		
		if (proctab[min_i].valid) {
			return proctab[min_i].pid; 
		}


	}
	
	return (0);
}


/*	HandleTimerIntr () is called by the kernel whenever a timer
 *	interrupt occurs.  
 */

void HandleTimerIntr ()
{
//Printf ("timer interrrupt occured \n");
	SetTimer (TIMERINTERVAL);       // resets the timer

	switch (GetSchedPolicy ()) {	// is policy preemptive?

	case ROUNDROBIN:		// ROUNDROBIN is preemptive
	case PROPORTIONAL:		// PROPORTIONAL is preemptive

		DoSched ();		// make scheduling decision
		break;

	default:			// if non-preemptive, do nothing
		break;
	}
}

/*	MyRequestCPUrate (pid, n) is called by the kernel whenever a process
 *	identified by pid calls RequestCPUrate (n). This is a request for
 *	n% of CPU time, i.e., requesting a CPU whose speed is effectively
 *	n% of the actual CPU speed.  Roughly n out of every 100 quantums
 *	should be allocated to the calling process.  n must be greater than
 *	0 and must be less than or equal to 100. MyRequestCPUrate (pid, n)
 *	should return 0 if successful, i.e., if such a request can be
 *	satisfied, otherwise it should return -1, i.e., error (including if
 *	n < 1 or n > 100).  If MyRequestCPUrate (pid, n) fails, it should
 *	have no effect on scheduling of this or any other process, i.e., AS
 *	IF IT WERE NEVER CALLED.
 */

int MyRequestCPUrate (pid, n)
	int pid;			// process whose rate to change
	int n;				// percent of CPU time
{
	/* your code here */

	if (n < 1 || n > 100) {
		return (-1);
	}


	//add requested time to total CPU allocated
	//if a process has made a request, check if the process's previous request+unallocated CPU time
	// can accomodate the new request


	int unallocated = 100 - allocated;

	//adjust stride value here? find the process in proctab
	for (int i = 0; i < MAXPROCS; i++) {
		if (proctab[i].valid && proctab[i].pid == pid) {
			
			//check if the process can make the request
		 	if (proctab[i].request >= n) {
		
				allocated = allocated-proctab[i].request;

				proctab[i].request = n;
				proctab[i].stridevalue = L/n;
		
				allocated = allocated+n;

				if (!proctab[i].requested) {
					proctab[i].requested = 1;
					requestedprocs++;
				}
			}			
			else {  //new request is greater than previous request
				if (proctab[i].request+unallocated >= n) {
					
					allocated = allocated-proctab[i].request;
					
					proctab[i].request = n;
					proctab[i].stridevalue = L/n;
					
					allocated = allocated+n;
				
					if (!proctab[i].requested) {
						proctab[i].requested = 1;
						requestedprocs++;
					}
				}
			}
		return (-1);

		}
	}

	//unused CPU time will be distributed to any remaining processes that have not requested CPU time

	//total number of processes that haven't made request

	
	int procs = totalprocs-requestedprocs;

	//new unallocated value
	unallocated = 100 - allocated;


	//if there are any processes that haven't made a request, give CPU time to them
	if (procs > 0) {
		int time = (unallocated)/procs;


		for (int i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid && !proctab[i].requested) {
				proctab[i].stridevalue = L/time;		

			}
		}
	}
	else {  //every process has made a request, so share the extra unallocated time evenly
		int time = (unallocated)/(totalprocs-1);

		for (int i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) {
				proctab[i].stridevalue = L/time;		

			}
		}
	}

	
	return (0);
}



