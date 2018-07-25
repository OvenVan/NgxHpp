
#include <nginx.h>

extern "C"{
	#include <ngx_http.h>
	#include <ngx_string.h>
}

#ifndef __NGINX_HPP__
#define __NGINX_HPP__

bool IsNull(void* p){
	return (p == nullptr) ? true : false;
}

#endif
