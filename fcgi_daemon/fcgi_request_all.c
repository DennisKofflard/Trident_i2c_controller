/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-02-21
 *
 *---------------------------------------------------------------------------
 */




#include <stdio.h>
#include <stdlib.h>
#include <fcgiapp.h>
#include <assert.h>
#include <string.h>

#include "i2c_devices.h"

#include "fcgi_helper.h"
#include "macros.h"

//#include "fcgi_request.h"
//#include "fcgi_request_debug.h"
//#include "fcgi_request_das.h"
#include "fcgi_request_all.h"




int fcgi_request_all_dispatch(FCGX_Request *request, char *url) {
	if (fcgi_request_strcmp(url, "debug")) {
		//logging:
		fprintf(stderr, "Dispatching to /all/dump\n");
		return fcgi_request_all_dump(request);
	
	} else {
		printf("command not found.\n");
		return -1;
	}
}



int fcgi_request_all_dump(FCGX_Request *request) {
	printf("/all/dump here.\n");

	return 0;
}
