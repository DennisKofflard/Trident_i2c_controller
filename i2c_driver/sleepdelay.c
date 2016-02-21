/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-02
 *
 *---------------------------------------------------------------------------
 */


#include <time.h>
#include <stdint.h>
#include <stdio.h>

#include "sleepdelay.h"


/**
 * function to suspend the execution of the program for a certain time.
 *
 * function sleeps for the total time specified by both parameters
 */
int sleepdelay(long seconds, long nanoseconds) {
  struct timespec t;
    t.tv_sec  = seconds;
    t.tv_nsec = nanoseconds;

  if ((nanoseconds >= (1 * 1000*1000*1000)) ||
			(nanoseconds < 0) ||
	    (seconds < 0)) {
		printf("wrong timespec. s: %ld, ns: %ld\n", seconds, nanoseconds);
		return -1;
	}


	//if nanosleep is interupted it will write
	//  the remaining time back in 't' and restart
	while(nanosleep(&t, &t));

	return 0;
}



/**
 * wrapper for #sleepdelay to facilitate sleeping a amount of time specified in microseconds
 *
 * \see #sleepdelay
 */
int sleepdelay_us(unsigned long microseconds) {
	long seconds;
	long nanoseconds;

	seconds     =         microseconds / (1000*1000);
	nanoseconds = 1000 * (microseconds % (1000*1000));

	return sleepdelay(seconds, nanoseconds);
}


/**
 * wrapper for #sleepdelay to facilitate sleeping a amount of time specified in miliseconds
 *
 * \see #sleepdelay
 */
int sleepdelay_ms(uint32_t miliseconds) {
	return sleepdelay_us(1000 * miliseconds);
}
