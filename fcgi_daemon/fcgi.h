/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_H
#define FCGI_H

#include <fcgiapp.h>

void handle_request(FCGX_Request *request);
int fcgi_start();
int fcgi_i2c_devices_call_on_type(i2c_dev_type type, FCGX_Request *request, int (*func)(i2c_dev*, FCGX_Request *));


#endif /* FCGI_H */
