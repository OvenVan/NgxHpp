#include "Nginx.hpp"
#include <new>

#ifndef __NGX_POOL_HPP__
#define __NGX_POOL_HPP__

class NgxPool{
private:
	ngx_pool_t* m_pool;

	template<typename T>
	static void destory(T* p)
	{
		(p) -> ~T();
	}

public:
	NgxPool(ngx_pool_t* p):m_pool(p)
	{}

	template<typename T>
	NgxPool(T* x):m_pool(x->pool)
	{}

	template<typename T>
	T* alloc() const
	{
		auto p = ngx_pcalloc(m_pool, sizeof(T));
		if	(p == nullptr) return nullptr;
		return new(p) T();
	}

	template<typename T>
	T* alloc(size_t n) const
	{
		auto p = ngx_pnalloc(m_pool, n);
		if	(p == nullptr) return nullptr;
		return reinterpret_cast<T*>(p);
	}


	template<typename F, typename T>
	ngx_pool_cleanup_t* cleanup(F func, T* data) const
	{
		ngx_pool_cleanup_t* p = ngx_pool_cleanup_add(m_pool, 0);
		if (p == nullptr) return nullptr;
		p->handler = reinterpret_cast<ngx_pool_cleanup_pt>(func);
		p->data = data;
		return p;
	}

	ngx_pool_cleanup_t* cleanup(ngx_pool_cleanup_pt func, size_t size) const
	{
		auto p = ngx_pool_cleanup_add(m_pool, size);
		if	(p == nullptr) return nullptr;
		p->handler = func;
		return p;
	}

	template<typename T>
	void cleanup(T* data) const
	{
		cleanup(&NgxPool::destory<T>, data);
	}
};

#endif
