/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-02
 *
 *---------------------------------------------------------------------------
 */


#ifndef SLEEPDELAY_H
#define SLEEPDELAY_H


int sleepdelay(long seconds, long nanoseconds);
int sleepdelay_us(unsigned long microseconds);
int sleepdelay_ms(uint32_t miliseconds);


#endif /* SLEEPDELAY_H */


