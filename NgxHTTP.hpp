#include "NgxHTTPResponse.hpp"
#include "NgxHTTPRequest.hpp"
#include "NgxPool.hpp"

#ifndef __NGX_HTTP_HPP__
#define __NGX_HTTP_HPP__

class NgxHTTP
{
public:
	NgxHTTPResponse set;
	NgxHTTPRequest get;

	NgxHTTP(ngx_http_request_t* r)
	{
	//	set = new(ngx_pcalloc(r->pool, sizeof(NgxHTTPResponse))) NgxHTTPResponse(r);
	//	get = new(ngx_pcalloc(r->pool, sizeof(NgxHTTPRequest))) NgxHTTPRequest(r);
		get.set(r);
		set.set(r);
	}
};


#endif
