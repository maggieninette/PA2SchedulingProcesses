#include <stdio.h>
#include "aux.h"
#include "umix.h"

void Main ()
{
	int pid = 0;

	if ((pid = Fork ()) == 0) {

		/* first child executes here */
		Printf ("I am the first child, my pid is %d\n", Getpid ());
		Exit ();
	}

	Printf ("I am the parent, my pid is %d\n", Getpid ());
	Printf ("I just created a child process whose pid is %d\n", pid);

	if ((pid = Fork ()) == 0) {

		/* second child executes here */
		Printf ("I am the second child, my pid is %d\n", Getpid ());
		Exit ();
	}

	/* HERE */

	Printf ("I (the parent) just created a second child process whose pid is %d\n", pid);
}
