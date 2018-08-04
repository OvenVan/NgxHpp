#include "Nginx.hpp"
#include "NgxPool.hpp"

#ifndef __NGX_STRING_HPP__
#define __NGX_STRING_HPP__
	
__attribute__((unused)) 
static const char* toStr(ngx_str_t* nstr, ngx_pool_t* m_p)
{
	u_char* ustr = NgxPool(m_p).alloc<u_char>(nstr->len + 1);
	ngx_memcpy(ustr, nstr->data, nstr->len);
	ustr[nstr->len] = '\0';
	return (const char*) ustr;
}

__attribute__((unused)) 
static const char* toStr(ngx_str_t& nstr, ngx_pool_t* m_p) 
{
	return toStr(&nstr, m_p);
}

template<typename T>
__attribute__((unused))
static const char* toStr(ngx_str_t& nstr, T* m_t) 
{
	return toStr(&nstr, m_t->pool);
}

template<typename T>
__attribute__((unused)) 
static const char* toStr(ngx_str_t* nstr, T* m_t) 
{
	return toStr(nstr, m_t->pool);
}

__attribute__((unused)) 
static ngx_str_t toNgxstr(const char* str)
{
	ngx_str_t rtn_str;
	rtn_str.len = strlen(str);
	rtn_str.data = (u_char*) str;
	return rtn_str;
}

class NgxString
{
private:
	u_char* m_str;
	bool isAlloc;  	//not useful.  current string uses the memory space or not
	size_t allocSize;
	u_char* allocSpace;

	void init()
	{
		m_str = nullptr;
		isAlloc = false;
		allocSize = 0;
		allocSpace = nullptr;
	}

public:
	template<typename T>
	NgxString(const char* str, T* m_r)	
	{							
		init();
		set(str, m_r);
	}

	template<typename T>
	NgxString(ngx_str_t* ngxstr, T* m_r)
	{
		init ();
		set(ngxstr, m_r);
	}

	NgxString(const char* str)
	{
		init();
		set(str);
	}

	NgxString()
	{
		init();
	}
	
	const char* get() const
	{
		return (const char*)m_str;
	}

	template<typename T>
	ngx_int_t set(const char* str, T* m_r)
	{
		size_t length = strlen(str) + 1;		
		if ((m_str == nullptr) || (length > allocSize))
		{			//realloc
			m_str = (u_char*)ngx_pcalloc(m_r->pool, length);
			ngx_memcpy(m_str, str, length);				//including \0
			allocSize = length;
			allocSpace = m_str;
		}
		else
		{
			ngx_memcpy(allocSpace, str, length);
		}
		isAlloc = true;

		return NGX_OK;
	}

	ngx_int_t set(const char* str)
	{
		m_str = (u_char*)str;
		isAlloc = false;
		return NGX_OK;
	}

	template<typename T>
	ngx_int_t set(ngx_str_t* ngxstr, T* m_r)
	{
		size_t length = ngxstr->len + 1;	

		if ((m_str == nullptr) || (length > allocSize))
		{			//realloc
printf("realloc %s\n", toStr(ngxstr, m_r));
			m_str = (u_char*)toStr(ngxstr, m_r);

			allocSize = length;
			allocSpace = m_str;
		}
		else
		{
			ngx_memcpy(allocSpace, ngxstr->data, length - 1);
			allocSpace[length -1] = '\0';
		}
		isAlloc = true;
		return NGX_OK;
	}

	size_t getLength()
	{
		return strlen((const char*)m_str);
	}
};

#endif
