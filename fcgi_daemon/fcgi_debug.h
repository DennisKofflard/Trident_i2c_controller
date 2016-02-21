/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_DEBUG_H
#define FCGI_DEBUG_H



int fcgi_html_header(FCGX_Request *request);
int fcgi_html_footer(FCGX_Request *request);
int fcgi_debug_dump(FCGX_Request *request);
int fcgi_das(FCGX_Request *request);


#endif /* FCGI_DEBUG_H */
