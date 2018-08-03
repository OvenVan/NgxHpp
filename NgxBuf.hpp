#include "NgxFile.hpp"
#include "NgxString.hpp"
#include <sys/stat.h>
#include <unistd.h>

#ifndef __NGX_BUF_HPP__
#define __NGX_BUF_HPP__

class NgxBuf 
{
private:
	bool registered;
	ngx_pool_t* m_p;
	ngx_buf_t* m_b;

	size_t getTotalSize()
	{
		return (size_t)(m_b->end - m_b->start);
	}

	void create(ngx_pool_t* p, size_t size = 0)
	{
		if (size > 0)
		{
			m_b = ngx_create_temp_buf(p, size);
		}
		else 
		{
			//m_b = (ngx_buf_t*)ngx_palloc(r->pool, sizeof(ngx_buf_t));
			//both can work
			m_b = ngx_create_temp_buf(p, size);
			m_b->temporary = 0;
			m_b->memory = 0;
			m_b->mmap = 0;
		}
		registered = ((size == 0) ? false : true);
	}
	template<typename T>
	void create(T* r, size_t size = 0)
	{
		create(r->pool, size);
	}
public:
	template<typename T>
	NgxBuf(T* r, size_t size = 0)
	{
		m_p = r->pool;
		create(r, size);
	}

	template<typename T, typename F>
	NgxBuf(T* r, F& f)
	{
		m_p = r->pool;
		create(r, sizeof(f));
		fulfill(f);
	}

	NgxBuf (const NgxBuf& copyBuf)
	{
		if (copyBuf.m_b->in_file)
		{
			create(copyBuf.m_p);
			m_b->in_file = 1;
			m_b->file = copyBuf.m_b->file;
			m_b->file_pos = copyBuf.m_b->file_pos;
			m_b->file_last = copyBuf.m_b->file_last;
		}
		else
		{
			size_t length = copyBuf.getDataSize();
			create(copyBuf.m_p, length);
			ngx_memcpy(m_b->pos, copyBuf.m_b->pos, length);
			m_b->last = m_b->pos + length;
		}
	}

	NgxBuf()
	{}

	~NgxBuf()
	{}

	ngx_buf_t* get() const
	{
		return m_b;
	}

	u_char* getData() const
	{
		return m_b->pos;
	}

	size_t getDataSize() const
	{
		return (size_t)(m_b->last - m_b->pos);
	}
	
	bool isLast() const
	{
		return (m_b->last_buf || m_b->last_in_chain);
	}
	
	void finish(bool flag = true) const
	{
		m_b->last_buf = flag;
		m_b->last_in_chain = flag;
	}
	static void finish(ngx_buf_t* m_b, bool flag = true)
	{
		m_b->last_buf = flag;
		m_b->last_in_chain = flag;
	}
    
	template<typename T>
	ngx_int_t fulfill(T& _t)
	{
		size_t length = sizeof(_t); 
		if ((!registered) || (getTotalSize() < length))
		{
			return NGX_DECLINED;
		}
		ngx_memcpy(m_b->pos, &_t, length);
		m_b->last = m_b->pos + length;
		return NGX_OK;
	}

	ngx_int_t fulfill(NgxFile& _ngxfile)
	{
		ngx_file_t* _file = _ngxfile.get();

		if (_file->fd <=0)
		{
			return NGX_HTTP_NOT_FOUND;
		}
		m_b->in_file = 1;
		m_b->file = _file;
		m_b->file_pos = 0;
		m_b->file_last = _file->info.st_size;
		return NGX_OK;
	}

	ngx_int_t fulfill (const char* _str)
	{
		size_t length = strlen(_str); //without \0
		if ((!registered) || (getTotalSize() < length))
		{
			return NGX_DECLINED;
		}
		ngx_memcpy(m_b->pos, _str, length);
		m_b->last = m_b->pos + length;
		return NGX_OK;
	}

	ngx_int_t fulfill (NgxString& _ngxstr)
	{
		return fulfill(_ngxstr.get());
	}

	ngx_int_t fulfill (ngx_str_t* _ngxustr)
	{
		if ((!registered) || (getTotalSize() < _ngxustr->len))
		{
			return NGX_DECLINED;
		}
		ngx_memcpy(m_b->pos, _ngxustr, _ngxustr->len);
		m_b->last = m_b->pos + _ngxustr->len;
		return NGX_OK;
	}

	ngx_int_t fulfill (ngx_str_t& _ngxustr)
	{
		return fulfill (&_ngxustr);
	}
};

#endif
