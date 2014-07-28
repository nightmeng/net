#include "net/tcp/accept_request.h"
#include "net/tcp/socket_base.h"

using namespace net::tcp;

accept_request::accept_request(socket_base *sock):request_base(sock){
}

acceptor::acallback accept_request::callback(){
	return icb;
}

void accept_request::callback(acceptor::acallback icallback){
	icb = icallback;
}

int accept_request::type()const{
	return req_type::ACCEPT_REQ;
}
