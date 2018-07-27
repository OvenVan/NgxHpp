#include "Nginx.hpp"
#include "NgxBuf.hpp"
#include "NgxPool.hpp"

#ifndef __NGX_HTTP_REQUEST_HPP__
#define __NGX_HTTP_REQUEST_HPP__

class NgxHTTPRequest
{
private:
	ngx_http_request_t* m_request;

public:
	NgxHTTPRequest(ngx_http_request_t* r) : m_request(r)
	{}

	NgxHTTPRequest()
	{}
	
	void set(ngx_http_request_t* r)
	{
		m_request = r;
	}

	ngx_int_t discard()
	{
		return ngx_http_discard_request_body(m_request);
	}

	template<typename F>
	ngx_int_t read(F f) const
	{
		ngx_int_t rc = ngx_http_read_client_request_body(m_request, f);

		if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
		{
			return NGX_ERROR;
		}
		return NGX_DONE;
	}

	bool isMethod(ngx_int_t method)
	{
		return (m_request->method & method)	;
	}

	bool isnotMethod(ngx_int_t method)
	{
		return !isMethod(method);
	}

	const char* args()
	{
		return toStr(m_request->args, m_request);
	}
	size_t argsLength()
	{
		return m_request->args.len;
	}
	
	const char* uri()
	{
		return toStr(m_request->uri, m_request);
	}
	size_t uriLength()
	{
		return m_request->uri.len;
	}

	const char* exten()
	{
		return toStr(m_request->exten, m_request);
	}
	size_t extenLength()
	{
		return m_request->exten.len;
	}

};

#endif


