/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-14
 *
 *---------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcgiapp.h>
#include <assert.h>
#include <string.h>

#include "fcgi_helper.h"
#include "macros.h"

#include "fcgi_request.h"
#include "fcgi_request_debug.h"
#include "fcgi_debug.h"





int fcgi_request_debug_dispatch(FCGX_Request *request, char *url) {
	FCGI_NEXT_PARAM();

	if (fcgi_request_strcmp(url, "test")) {
		printf("debug/test: OK\n");
		return 0;

	} else if (fcgi_request_strcmp(url, "dump")) {
		return fcgi_request_debug_dump_dispatch(request, url);

	} else {
		printf("command not found.\n");
		return -1;
	}
}





int fcgi_request_debug_dump_dispatch(FCGX_Request *request, char *url) {
	FCGI_NEXT_PARAM();

	if (fcgi_request_strcmp(url, "all")) {
		return fcgi_debug_dump(request);

	} else {
		printf("command not found.\n");
		return -1;
	}
}
