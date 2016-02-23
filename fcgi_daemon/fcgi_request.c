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

#include "i2c_devices.h"

#include "fcgi_helper.h"
#include "macros.h"

#include "fcgi_request.h"
#include "fcgi_request_debug.h"
#include "fcgi_request_das.h"
#include "fcgi_request_all.h"




int fcgi_request(FCGX_Request *request) {
	char *url = get_param("REQUEST_URI");

	if (url == NULL) {
		//NO URI, whaaat ?
		fprintf(stderr, "no uri received from fcgi\n");
		return -1;
	}


	if (url[0] != '/') {
		fprintf(stderr, "webserver is doing something weird.\n");
		fprintf(stderr, "   uri received was: %s\n", url);
		return -1;
	} else {
		url++; //shift off first '/' character
	}


	//logging:
	fprintf(stderr, "uri received: %s\n", url);


	return fcgi_request_dispatch(request, url);
}




int fcgi_request_dispatch(FCGX_Request *request, char *url) {
	if (fcgi_request_strcmp(url, "debug")) {
		//logging:
		printf("Content-Type: text/plain\n\n");
		fprintf(stderr, "Dispatching to /debug\n");
		return fcgi_request_debug_dispatch(request, url);

	} else if (fcgi_request_strcmp(url, "das")) {
		printf("Content-Type: text/plain\n\n");
		fprintf(stderr, "Dispatching to /das\n");
		return fcgi_request_das_dispatch(request, url);

	} else if (fcgi_request_strcmp(url, "all")) {
		fprintf(stderr, "Dispatching to /all\n");
		return fcgi_request_all_dispatch(request, url);

	} else if (fcgi_request_strcmp(url, "banaan")) {
		printf("Content-Type: text/plain\n\n");
		printf("banaan!\n");
		return 0;

	} else {
		printf("command not found.\n");
		return -1;
	}
}











//compares str up to the next slash in url
int fcgi_request_strcmp(char *url, char *str) {
	char *next_slash = strchr(url, '/');

	size_t slen, ulen;


	if (next_slash == NULL)  {
		//fprintf(stderr, "no slash\n");
		ulen = strlen(url);
	} else {
		//fprintf(stderr, " a slash\n");
		ulen = next_slash - url;
	}

	slen = strlen(str);

	//fprintf(stderr, "url: %3zd, str: %3zd\n", ulen, slen);

	if (ulen != slen) {
		//fprintf(stderr, "string lengths unequal\n");
		return 0;
	} else  {
		//fprintf(stderr, "memcmp-ing\n");
		return (memcmp(url, str, slen) == 0);
	}
}







//cuts of the part up to and including the next slash to expose the next part.
int fcgi_request_next_part(char **url) {
	char *next_slash = strchr(*url, '/');

	if ((next_slash == NULL) ||
		(*(next_slash + 1) == '\0')) {
		//there were no slashes left or the string ended with a slash.
		*url = NULL;
		return -1;
	} else {
		*url = next_slash + 1; //offset beyond the slash.
		return 0;
	}
}





//converts a logical value to a string representation.
char* fcgi_request_bit_to_bool(uint8_t bit) {
	return (bit ? "true" : "false");
}







