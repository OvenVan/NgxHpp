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
		}
		registered = ((size == 0) ? false : true);
		setFile(false);
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
		create(r);
		fulfill(f);
	}

	template<typename T>
	NgxBuf(T* r, ngx_buf_t* _buf)
	{
		m_p = r->pool;
		m_b = _buf;
		registered = false;
	}

	NgxBuf(ngx_pool_t* _p, ngx_buf_t* _buf)
	{
		m_p = _p;
		m_b = _buf;
		registered = false;
	}

	NgxBuf (const NgxBuf& copyBuf)
	{
		if (copyBuf.isFile())
		{
			create(copyBuf.m_p);
			setFile();
			m_b->file = copyBuf.m_b->file;
			m_b->file_pos = copyBuf.m_b->file_pos;
			m_b->file_last = copyBuf.m_b->file_last;
		}
		else if (copyBuf.isOnlyRead())
		{
			create(copyBuf.m_p);
			setFile(false);
			m_b->pos = copyBuf.m_b->pos;
			m_b->last = copyBuf.m_b->last;
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
	
	void printInfo()
	{
		printf("buf:%p, last_buf:%d%d, file:%d, mem:%d\n", m_b, (int)m_b->last_buf, (int)m_b->last_in_chain, (int)m_b->in_file, (int)(m_b->temporary|m_b->memory));
	}
		
	ngx_buf_t* get() const
	{
		return m_b;
	}
	template<typename T = u_char>
	const T* getData() const
	{
		return m_b->pos;
	}
	size_t getDataSize() const
	{
		if (isFile())
		{
			return (m_b->file->info.st_size);
		}
		else
		{
			return (size_t)(m_b->last - m_b->pos);
		}
	}
	
	bool isFile() const
	{
		return (m_b->in_file);
	}
	void setFile(bool _file = true)
	{
		if (_file){
			m_b->temporary = 0;
			m_b->memory = 0;
			m_b->mmap = 0;
			m_b->in_file = 1;
		}	
		else
		{
			if (registered)
			{
				m_b->temporary = 1;
				m_b->memory = 0;
				m_b->mmap = 0;
				m_b->in_file = 0;
			}
			else
			{
				m_b->temporary = 0;
				m_b->memory = 1;
				m_b->mmap = 0;
				m_b->in_file = 0;
			}
		}
	}
	bool isOnlyRead() const
	{
		return (m_b->memory | m_b->mmap);
	}
	bool setOnlyRead(bool OR = true)
	{
		if (m_b->in_file)
		{
			return false;
		}

		if (OR)
		{
			m_b->temporary = 0;
			m_b->memory = 1;
			m_b->mmap = 0;
		}
		else
		{
			m_b->temporary = 1;
			m_b->memory = 0;
			m_b->mmap = 0;
		}
		return true;
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
	size_t fulfill(T& _t)
	{
		size_t length = sizeof(_t); 
		if ((!registered) || (getTotalSize() < length))
		{
			m_b->pos = (u_char*)&_t;
			setOnlyRead();
		}
		else
		{
			ngx_memcpy(m_b->pos, &_t, length);
			setOnlyRead(false);
		}
		m_b->last = m_b->pos + length;
		return length;
	}

	size_t fulfill (ngx_str_t* _ngxustr)
	{
		if ((!registered) || (getTotalSize() < _ngxustr->len))
		{
			m_b->pos = (u_char*)_ngxustr;
			setOnlyRead();
		}
		else
		{
			ngx_memcpy(m_b->pos, _ngxustr, _ngxustr->len);
			setOnlyRead(false);
		}
		m_b->last = m_b->pos + _ngxustr->len;
		return _ngxustr->len;
	}

	size_t fulfill (const char* _str)
	{
		size_t length = strlen(_str); //without \0
	
		if ((!registered) || (getTotalSize() < length))
		{
			m_b->pos = (u_char*)_str;
			setOnlyRead();
		}
		else
		{
			ngx_memcpy(m_b->pos, _str, length);
			setOnlyRead(false);
		}
		m_b->last = m_b->pos + length;
		return length;
	}

	size_t fulfill(NgxFile& _ngxfile)
	{
		ngx_file_t* _file = _ngxfile.get();
		if (_file->fd <=0)
		{
			return 0;
		}
		setFile();
		size_t length = _file->info.st_size;
		m_b->file = _file;
		m_b->file_pos = 0;
		m_b->file_last = length;
		return length;
	}

	size_t fulfill (NgxString& _ngxstr)
	{
		return fulfill(_ngxstr.get());
	}

	size_t fulfill (ngx_str_t& _ngxustr)
	{
		return fulfill (&_ngxustr);
	}
};

#endif
