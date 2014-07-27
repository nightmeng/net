#include "accept_request.h"

accept_request::accept_icallback accept_request::callback(){
	return icb;
}

void accept_request::callback(accept_request::accept_icallback icallback){
	icb = icallback;
}
