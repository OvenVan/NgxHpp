#include "Nginx.hpp"
#include "NgxBuf.hpp"
#include "NgxString.hpp"
#include "NgxFile.hpp"

#ifndef __NGX_HTTP_RESPONSE_HPP__
#define __NGX_HTTP_RESPONSE_HPP__

class NgxHTTPResponse{		//set
private:
	ngx_http_request_t* m_request;

	ngx_int_t send(ngx_chain_t* ch)
	{
		return ngx_http_output_filter(m_request, ch);
	}

	void ContentType(ngx_str_t _type)
	{
		m_request->headers_out.content_type = _type;
	}

public:
	NgxHTTPResponse(ngx_http_request_t* r) : m_request(r)
	{}

	NgxHTTPResponse()
	{}

	void set(ngx_http_request_t* r)
	{
		m_request = r;
	}

	void Status(ngx_int_t _status = NGX_HTTP_OK)
	{
		m_request->headers_out.status = _status;
	}

	void ContentLength(ngx_int_t _length)
	{
		m_request->headers_out.content_length_n = _length;
	}

	void ContentType(const char* _typestr)
	{
		ngx_str_t _str = toNgxstr(_typestr);
		ContentType(_str);
	}
	void ContentType(NgxString& _typestr)
	{
		ContentType(_typestr.get());
	}

	ngx_int_t send()
	{
		if (m_request->header_sent)
		{
			return NGX_OK;
		}
		ngx_int_t rc = ngx_http_send_header(m_request);
		if (rc == NGX_ERROR || rc > NGX_OK || m_request->header_only)
		{
			return rc;
		}
		return NGX_OK;
	}

	ngx_int_t send(NgxBuf& _ngxBuf)
	{
		ngx_buf_t* b = _ngxBuf.get();
		b->last_buf = 1;
		ngx_chain_t out;
		out.buf = b;
		out.next = NULL;
		return send(&out);
	}

	ngx_int_t send(NgxString& _ngxStr)
	{
		NgxBuf buf(m_request, _ngxStr.getLength());
		buf.fulfill(_ngxStr);
		return send(buf);
	}

	ngx_int_t send(NgxFile& _ngxFile)
	{

		NgxBuf buf(m_request);
		buf.fulfill(_ngxFile);
		auto rc = send(buf);
		return rc;
	}

	ngx_int_t flush() const
	{
		return ngx_http_send_special(m_request, NGX_HTTP_FLUSH);
	}

	ngx_int_t eof() const
	{
		return ngx_http_send_special(m_request, NGX_HTTP_LAST);
	}

	void finalize(ngx_int_t rc = NGX_HTTP_OK) const
	{
		ngx_http_finalize_request(m_request, rc);
	}

};

#endif
