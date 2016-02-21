/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcgiapp.h>
#include <assert.h>
#include <sys/stat.h>

#include "i2c_devices.h"
#include "reports.h"
#include "macros.h"

#include "fcgi_helper.h"
#include "fcgi_debug.h"
#include "fcgi_request.h"




void handle_request(FCGX_Request *request) {
	//printf("Content-Type: text/plain\r\n\r\n");
	printf("Content-Type: text/html\r\n\r\n");


	/*
	if (
	fcgi_html_header(request) ||

		fcgi_request(request) ||

		//does not care about request, just dump everything.
		//fcgi_debug_dump(request);
		//fcgi_debug_dump(request) ||

	fcgi_html_footer(request)) {
	*/
		
	if (fcgi_request(request)) {
		printf("handling request failed.\n");
	}

	printf("\n");
}



int fcgi_start() {
	int sock;
	FCGX_Request request;

	FCGX_Init();
	sock = FCGX_OpenSocket("/tmp/fcgi_sock", 5);
	//set a=rwx on socket file
	chmod("/tmp/fcgi_sock", S_IRWXU|S_IRWXG|S_IRWXO);

	FCGX_InitRequest(&request, sock, 0);

	while (FCGX_Accept_r(&request) >= 0) {
		handle_request(&request);
		FCGX_Finish_r(&request);
	}


	//should not ever return
	assert("fcgi loop returned" == 0);
	return -1;
}






int fcgi_i2c_devices_call_on_type(i2c_dev_type type, FCGX_Request *request, int (*func)(i2c_dev*, FCGX_Request *)) {
	int i = 0;
	i2c_dev *dev;

	while (!(i2c_devices_dev_get(&dev, type, i))) {
		DO_AND_CHECK((*func)(dev, request));
		i++;
	}

	return 0;
}
