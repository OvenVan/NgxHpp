#include "NgxBuf.hpp"

#ifndef __NGX_CHAIN_HPP__
#define __NGX_CHAIN_HPP__

class NgxChain
{
private:
	ngx_chain_t* m_chain;			
	ngx_chain_t* m_end;
	ngx_pool_t* m_pool;

	void init(ngx_pool_t* p)
	{
		if (!m_pool)
		{
			m_pool = p;
			m_chain = nullptr;
			m_end = nullptr;
		}
	}
	void init()
	{
		m_pool = nullptr;
		m_chain = nullptr;
		m_end = nullptr;
	}

	ngx_buf_t* check(NgxBuf& _buf)
	{
		for (ngx_chain_t* i = m_chain; i != nullptr; i = i->next)
		{
			if (i->buf == _buf.get())
			{
				return NgxBuf(_buf).get();
			}
		}
		return _buf.get();
	}
public:
	NgxChain(ngx_pool_t* p, ngx_chain_t* ch = nullptr)
	{
		init();
		init(p);
		if (ch)
		{
			m_chain = ch;
			for (m_end = ch; m_end->next != nullptr; m_end = m_end->next){}
		}
	}

	template<typename T>
	NgxChain(T* x, ngx_chain_t* ch = nullptr)
	{
		init();
		init(x->pool);
		if (ch)
		{
			m_chain = ch;
			for (m_end = ch; m_end->next != nullptr; m_end = m_end->next){}
		}
	}

	~NgxChain() = default;
	
	void set(ngx_chain_t* ch)
	{
		if (m_chain == nullptr)
		{
			m_chain = ch;
			for (m_end = ch; m_end->next != nullptr; m_end = m_end->next){}
		}
	}

	ngx_chain_t* get()
	{
		return m_chain;
	}
	
	bool push_back(NgxBuf& _buf, bool insist = false)
	{
		ngx_buf_t* checkBuf = check(_buf);
		if (checkBuf != _buf.get())
		{
			if (!insist)
			{
				return false;
			}
		}
		ngx_chain_t* _chain = NgxPool(m_pool).alloc<ngx_chain_t>();
		_buf.finish();
		_chain->buf = checkBuf;
		_chain->next = nullptr;
		if (m_chain == nullptr)
		{
			m_chain = _chain;
		}
		else
		{
			m_end->next = _chain;
			NgxBuf::finish(m_end->buf, false);
		}
		m_end = _chain;
		return true;
	}

	bool push_front(NgxBuf& _buf, bool insist = false)
	{
		ngx_buf_t* checkBuf = check(_buf);
		if (checkBuf != _buf.get())
		{
			if (!insist)
			{
				return false;
			}
		}
		ngx_chain_t* _chain = NgxPool(m_pool).alloc<ngx_chain_t>();
		_chain->buf = checkBuf;
		_chain->next = nullptr;
		if (m_chain == nullptr)
		{
			m_end = _chain;
		}
		else
		{
			_chain->next = m_chain;
		}
		m_chain = _chain;
		return true;
	}
	
};

#endif	
