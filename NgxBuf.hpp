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
	ngx_buf_t* m_b;

	size_t getTotalSize()
	{
		return (size_t)(m_b->end - m_b->start);
	}
	
public:
	template<typename T>
	NgxBuf(T* r, size_t size = 0)
	{
		if (size > 0)
		{
			m_b = ngx_create_temp_buf(r->pool, size);
		}
		else 
		{
			//m_b = (ngx_buf_t*)ngx_palloc(r->pool, sizeof(ngx_buf_t));
			//both can work
			m_b = ngx_create_temp_buf(r->pool, size);
			m_b->temporary = 0;
			m_b->memory = 0;
			m_b->mmap = 0;
		}
//printf("ngxbuf c: start:%p, pos:%p, last:%p, end:%p\n", m_b->start, m_b->pos, m_b->last, m_b->end);
		registered = ((size == 0) ? false : true);
	}

	~NgxBuf()
	{}

	ngx_buf_t* get()
	{
		return m_b;
	}

	u_char* getData()
	{
		return m_b->pos;
	}

	size_t getDataSize()
	{
		return ngx_buf_size(m_b);
	}
	
	bool last() const
	{
		return (m_b->last_buf || m_b->last_in_chain);
	}

	void finish(bool flag = true) const
	{
		m_b->last_buf = flag;
		m_b->last_in_chain = flag;
	}
     
	ngx_int_t fulfill(NgxFile& _ngxfile)
	{
		ngx_file_t* _file = _ngxfile.get();

		if (_file->fd <=0)
		{
			return NGX_HTTP_NOT_FOUND;
		}
printf("fulfill: name.data = %s\n", (const char*)_file->name.data);
printf("fulfill: m_filename = %s\n", _ngxfile.getPath());
		if (fstat(_file->fd, &(_file->info)) == NGX_FILE_ERROR)
		{
printf("error: %s\n", strerror(errno));
			return NGX_HTTP_INTERNAL_SERVER_ERROR;
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
//printf("ngx_buf.fulfill_str: start:%p, pos:%p, last:%p, end:%p\n", m_b->start, m_b->pos, m_b->last, m_b->end);

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
