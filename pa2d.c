//pa2d.c
#include <stdio.h>
#include "aux.h"
#include "umix.h"

void Main ()
{
	if (Fork () == 0) {//
		
		RequestCPUrate (30);				// process 2
		SlowPrintf (7, "222222222222222222");

		Exit ();
	}

	if (Fork () == 0) {

		RequestCPUrate (20);				// process 3
		SlowPrintf (7, "333333333333333333");
	
		Exit ();
	}
  
  if (Fork () == 0) {

			RequestCPUrate (10);			// process 4
			SlowPrintf (7, "444444444444444444");

			Exit ();
		}

  if (Fork () == 0) {

			                    			// process 5
			SlowPrintf (7, "555555555555555555");

			Exit ();
		}
  if (Fork () == 0) {

			RequestCPUrate (40);			// process 6
			SlowPrintf (7, "6666666666666666666");

			Exit ();
		}

	RequestCPUrate (40);					// process 1
	SlowPrintf (7, "111111111111111111");
	
	Exit ();
}
