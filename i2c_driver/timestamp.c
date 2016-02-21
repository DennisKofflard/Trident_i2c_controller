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

#include "timestamp.h"
#include "macros.h"




/**
 * Gets a fresh timestamp from the kernel.
 *
 * @param[out] timestamp
 */
int timestamp_get(timespec *timestamp) {
	return clock_gettime(CLOCK_MONOTONIC, timestamp);
}


/**
 * corrects a timespec according to the time.h definition.
 * makes sure 0 <= tv_nsec <= 999 999 999
 * by adjusting tv_sec and tv_nsec in unison.
 *
 * @param[in,out] X
 */
int timestamp_correct(timespec *X) {
	//fprintf(stderr, "timestamp before correction:  %5lds, %9ldns\n", X->tv_sec, X->tv_nsec);

	X->tv_sec  += X->tv_nsec / NSEC_IN_A_SEC;
	X->tv_nsec  = X->tv_nsec % NSEC_IN_A_SEC;

	//fprintf(stderr, "timestamp between correction: %5lds, %9ldns\n", X->tv_sec, X->tv_nsec);
	
	//correct remainer operator to modulus
	if (X->tv_nsec < 0) {
		X->tv_nsec += NSEC_IN_A_SEC;
		X->tv_sec -= 1;
	}
	
	//fprintf(stderr, "timestamp after correction:   %5lds, %9ldns\n", X->tv_sec, X->tv_nsec);

	return 0;
}


/**
 * Substracts timestamp B from timestamp A
 *
 *
 * @param[in]  A
 * @param[in]  B
 * @param[out] result
 */
int timestamp_subtract(const timespec *A, const timespec *B, timespec *result) {
	result->tv_sec  = A->tv_sec  - B->tv_sec;
	result->tv_nsec = A->tv_nsec - B->tv_nsec;

	DO_AND_CHECK(timestamp_correct(result));

	return 0;
}


/**
 * adds timestamp B to timestamp A
 *
 *
 * @param[in]  A
 * @param[in]  B
 * @param[out] result
 */
int timestamp_add(const timespec *A, const timespec *B, timespec *result) {
	result->tv_sec  = A->tv_sec  + B->tv_sec;
	result->tv_nsec = A->tv_nsec + B->tv_nsec;

	DO_AND_CHECK(timestamp_correct(result));

	return 0;
}


/**
 * Compares timestamp A to timestamp B
 *
 * @param[in]  A
 * @param[in]  B
 * 
 * \returns (A>B) = -1, (A==B) = 0, (A<B) = +1
 */
int timestamp_compare(const timespec *A, const timespec *B) {
	timespec difference;
	timestamp_subtract(B, A, &difference);

	if ((difference.tv_sec > 0) ||
	   ((difference.tv_sec == 0) && (difference.tv_nsec > 0))) {
		return 1;
	} else if ((difference.tv_sec == 0) && (difference.tv_nsec == 0)) {
		return 0;
	} else {
		return -1;
	}
}

//check if time with offset has passed by now.
/**
 * compares (A + offset) with (the current time)
 *
 * @param[in]  A
 * @param[in]  offset
 *
 * \returns (A > now) = -1, (A == now) = 0, (A < now) = +1
 */
int timestamp_compare_offset(const timespec *A, const timespec *offset) {
	timespec A_offset;
	timespec now;

	timestamp_add(A, offset, &A_offset);
	timestamp_get(&now);

	return timestamp_compare(&A_offset, &now);
}















