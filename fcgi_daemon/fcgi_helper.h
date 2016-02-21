/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_HELPER_H
#define FCGI_HELPER_H



#define printf(...) FCGX_FPrintF(request->out, __VA_ARGS__)
#define get_param(KEY) FCGX_GetParam(KEY, request->envp)



#define FCGI_NEXT_PARAM()  if (fcgi_request_next_part(&url)) {printf("url ended early\n");	return -1;}


#endif /* FCGI_HELPER_H */
