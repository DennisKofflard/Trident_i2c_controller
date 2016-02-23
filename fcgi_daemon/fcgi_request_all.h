/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-02-21
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_REQUEST_ALL_H
#define FCGI_REQUEST_ALL_H



int fcgi_request_all_dispatch(FCGX_Request *request, char *url);
int fcgi_request_all_dump(FCGX_Request *request);
int fcgi_request_all_general(FCGX_Request *request);

int fcgi_request_transceivers(FCGX_Request *request);
int fcgi_request_transceiver_single(i2c_dev* dev, FCGX_Request *request);


#endif /* FCGI_REQUEST_ALL_H*/

