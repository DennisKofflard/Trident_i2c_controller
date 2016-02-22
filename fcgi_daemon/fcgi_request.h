/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-14
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_REQUEST_H
#define FCGI_REQUEST_H


int fcgi_request(FCGX_Request *request);
int fcgi_request_dispatch(FCGX_Request *request, char *url);

int fcgi_request_strcmp(char *url, char *str);
int fcgi_request_next_part(char **url);


#endif /* FCGI_REQUEST_H */
