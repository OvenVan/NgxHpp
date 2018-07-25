#include "Nginx.hpp"
#include "NgxBuf.hpp"

#ifndef __NGX_HTTP_RESPONSE_HPP__
#define __NGX_HTTP_RESPONSE_HPP__

class NgxHTTPResponse{
private:
	ngx_http_request_t* m_request;

public:
	NgxHTTPResponse(ngx_http_request_t* r) : m_request(r)
	{}

	void setStatus(ngx_int_t _status = NGX_HTTP_OK)
	{
		m_request->headers_out.status = _status;
	}

	void setContentLength(ngx_int_t _length)
	{
		m_request->headers_out.content_length_n = _length;
	}

	void setContentType(ngx_str_t _type)
	{
		m_request->headers_out.content_type = _type;
	}

	void setContentType(const char* _typestr)
	{
		setContentType(ngx_string(_typestr));
	}

	ngx_int_t sendHeader()
	{
		ngx_int_t rc = ngx_http_send_header(m_request);
		if (rc == NGX_ERROR || rc > NGX_OK || m_request->header_only)
		{
			return rc;
		}
		return NGX_OK;
	}

	ngx_int_t sendPackage(ngx_chain_t* ch)
	{
		return ngx_http_output_filter(m_request, ch);
	}

	ngx_int_t sendPackage(NgxBuf& _ngxBuf)
	{
		ngx_buf_t* b = _ngxBuf.get();
		b->last_buf = 1;
		ngx_chain_t out;
		out.buf = b;
		out.next = NULL;
		return sendPackage(&out);
	}
};

#endif
