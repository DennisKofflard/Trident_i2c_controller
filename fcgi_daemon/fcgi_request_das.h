/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-14
 *
 *---------------------------------------------------------------------------
 */


#ifndef FCGI_REQUEST_DAS_H
#define FCGI_REQUEST_DAS_H


int fcgi_request_das_dispatch(FCGX_Request *request, char *url);
int fcgi_das_single_output(i2c_dev* dev, FCGX_Request *request, uint8_t channel);
int fcgi_i2c_das_dump_all(FCGX_Request *request);



#endif /* FCGI_REQUEST_DAS_H */
