#include "Nginx.hpp"
#include "NgxPool.hpp"

#ifndef __NGX_STRING_HPP__
#define __NGX_STRING_HPP__
	
const char* toStr(ngx_str_t* nstr, ngx_pool_t* m_p)
{
	u_char* ustr = NgxPool(m_p).alloc<u_char>(nstr->len + 1);
	ngx_memcpy(ustr, nstr->data, nstr->len);
	ustr[nstr->len] = '\0';
	return (const char*) ustr;
}

const char* toStr(ngx_str_t& nstr, ngx_pool_t* m_p) 
{
	return toStr(&nstr, m_p);
}

template<typename T>
const char* toStr(ngx_str_t& nstr, T* m_t) 
{
	return toStr(&nstr, m_t->pool);
}

template<typename T>
const char* toStr(ngx_str_t* nstr, T* m_t) 
{
	return toStr(nstr, m_t->pool);
}
//it's noot good. if we change m_str too often, it will cause a lot of memory waste.
class NgxString
{
private:
	u_char* m_str;
	bool isAlloc;
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
	NgxString(const char* str, T* m_r)						//it depends, if *str is created by user, then 
	{														//we don't need to malloc a new memory space.
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
	
	const char* get()
	{
		return (const char*)m_str;
	}

	template<typename T>
	ngx_int_t set(const char* str, T* m_r)
	{
		size_t length = strlen(str) + 1;					//however if *str is just an address from nginx

		if ((m_str == nullptr) || (length > allocSize))
		{			//realloc
printf("realloc: %s\n", str);
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
		size_t length = ngxstr->len + 1;					//however if *str is just an address from nginx

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
