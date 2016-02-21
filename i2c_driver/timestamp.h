/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-02
 *
 *---------------------------------------------------------------------------
 */


#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <time.h>

#define NSEC_IN_A_SEC  (1000*1000*1000)
#define NSEC_IN_A_MSEC (0001*1000*1000)
#define NSEC_IN_A_USEC (0001*0001*1000)

typedef struct timespec timespec;


int timestamp_get(timespec *timestamp);
int timestamp_correct(timespec *X);
int timestamp_subtract(const timespec *A, const timespec *B, timespec *result);
int timestamp_add(const timespec *A, const timespec *B, timespec *result);
int timestamp_compare(const timespec *A, const timespec *B);
int timestamp_compare_offset(const timespec *A, const timespec *offset);


#endif /* TIMESTAMP_H */


