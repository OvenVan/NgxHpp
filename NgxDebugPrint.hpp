
#include <stdio.h>
#include "NgxString.hpp"

#ifndef __NGX_DEBUG_PRINT_HPP__
#define __NGX_DEBUG_PRINT_HPP__

class NgxDebugPrint
{
private:
	ngx_http_request_t *m_r;

public:
	NgxDebugPrint(ngx_http_request_t* r): m_r(r)
	{}

	void printRequest()
	{
		printf("\n");
		printf("method_name: %s\n", toStr(m_r->method_name, m_r));
		printf("http_protocol: %s\n", toStr(m_r->http_protocol, m_r));
		printf("request_line: %s\n", toStr(m_r->request_line, m_r));
		printf("uri: %s\n", toStr(m_r->uri, m_r));
		printf("exten: %s\n", toStr(m_r->exten, m_r));
		printf("args(url): %s\n", toStr(m_r->args, m_r));
		printf("\n");
	}
};

#endif 
