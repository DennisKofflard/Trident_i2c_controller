/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-14
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_REQUEST_DEBUG_H
#define FCGI_REQUEST_DEBUG_H


int fcgi_request_debug_dispatch(FCGX_Request *request, char *url);
int fcgi_request_debug_dump_dispatch(FCGX_Request *request, char *url);



#endif /* FCGI_REQUEST_DEBUG_H */
