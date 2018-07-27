#include "NgxPool.hpp"


#ifndef __NGX_ARRAY_HPP__
#define __NGX_ARRAY_HPP__

template <typename T>
class NgxArray
{
private:
	ngx_array_t* m_array;

	void array(ngx_uint_t n, ngx_pool_t* p)
	{
		if (m_array == nullptr)
		{
			m_array = ngx_array_create(p, n, sizeof(T));
		}
	}

	T* elts() const
	{
		return reinterpret_cast<T*>(m_array->elts);
	}

	T& prepare() const
	{
		auto tmp = ngx_array_push(m_array);
		return *reinterpret_cast<T*>(tmp);
	}

public:
	NgxArray(ngx_uint_t n, ngx_pool_t *p)
	{
		m_array = nullptr;
		array(n, p);
	}

	template <typename P>
	NgxArray(ngx_uint_t n, P* p)
	{
		m_array = nullptr;
		array(n, p->pool);
	}

	ngx_uint_t size() const
	{
		return m_array?m_array->nelts:0;
	}

	T& operator[](ngx_uint_t i) const
	{
		return elts()[i];
	}

	void push(const T& x) const
	{
		prepare() = x;
	}
};



#endif
