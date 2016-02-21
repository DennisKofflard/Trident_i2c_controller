/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */


#ifndef POLL_H
#define POLL_H


int poll_all();
void *thread_poll_wrapper(void *data_ptr);
void thread_poll_start();

#endif /* POLL_H */
