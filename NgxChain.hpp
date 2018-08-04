#include "NgxBuf.hpp"

#ifndef __NGX_CHAIN_HPP__
#define __NGX_CHAIN_HPP__

class NgxChain
{
private:
	ngx_chain_t* m_chain;			
	ngx_chain_t* m_end;
	ngx_pool_t* m_p;

	void init(ngx_pool_t* p)
	{
		m_p = p;
		m_chain = nullptr;
		m_end = nullptr;
	}
	void init(ngx_pool_t* p, ngx_chain_t* ch = nullptr)
	{
		init(p);
		if (ch)
		{
			m_chain = ch;
			for (m_end = ch; m_end->next != nullptr; m_end = m_end->next){}
		}
	}

	ngx_buf_t* check(const NgxBuf& _buf)
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
		init(p, ch);
	}
	template<typename T>
	NgxChain(T* x, ngx_chain_t* ch = nullptr)
	{
		init(T->pool, ch);
	}
	~NgxChain() = default;
	
	void printAll()
	{
		int count = 0;
		for (auto i = m_chain; i != nullptr; i = i->next)
		{
			NgxBuf(m_p, i->buf).printInfo();
			++count;
		}
		printf("m_chain is %p, m_end is %p\n, m_end->next is %p\n", m_chain, m_end, m_end->next);
	}

	bool set(ngx_chain_t* ch)
	{
		if (m_chain != nullptr)
		{
			return false;
		}
		m_chain = ch;
		for (m_end = ch; m_end->next != nullptr; m_end = m_end->next){}
		return true;
	}

	ngx_chain_t* get()
	{
		return m_chain;
	}

	bool push_back(const NgxBuf& _buf, bool insist = true)
	{
		ngx_buf_t* checkBuf = check(_buf);
		ngx_chain_t* _chain = nullptr;
		if ((checkBuf != _buf.get()) && (!insist))
		{
			return false;
		}
//printf("in pushback: &_buf = %p %ld, checkBuf = %p %ld\n", _buf.get(), _buf.getDataSize(), checkBuf, NgxBuf(m_p, checkBuf).getDataSize()); 
		_chain = NgxPool(m_p).alloc<ngx_chain_t>();
		NgxBuf::finish(checkBuf, true);
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

	bool push_front(const NgxBuf& _buf, bool insist = true)
	{
		ngx_buf_t* checkBuf = check(_buf);
		ngx_chain_t* _chain = nullptr;
		if (checkBuf != _buf.get())
		{
			if (!insist)
			{
				return false;
			}
		}
//printf("in pushfront: &_buf = %p %ld, checkBuf = %p %ld\n", _buf.get(), _buf.getDataSize(), checkBuf, NgxBuf(m_p, checkBuf).getDataSize());
		_chain = NgxPool(m_p).alloc<ngx_chain_t>();
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
