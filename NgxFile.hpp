#include "NgxPool.hpp"
#include "NgxString.hpp"
#include <stdio.h>
#include <string.h>

#ifndef __NGX_FILE_HPP__
#define __NGX_FILE_HPP__

class NgxFile{
private:
	ngx_file_t* m_file;
	const char* m_filename;
	
	template<typename T>
	ngx_int_t set(const u_char* filepath, T* r, ngx_int_t oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, ngx_int_t mode = NGX_FILE_OPEN, ngx_int_t access = 0)
	{
		m_file = NgxPool(r->pool).alloc<ngx_file_t>();
		m_file->fd = ngx_open_file(filepath, oflag, NGX_FILE_OPEN, access);
		m_file->name.data = (u_char*)filepath;
		m_file->name.len = strlen((const char*)filepath);
		m_file->log = r->connection->log;

		m_filename = toStr(m_file->name, r);

		if (fstat(m_file->fd, &(m_file->info)) == NGX_FILE_ERROR)
		{
//printf("NgxFile set: fstat error %s\n", strerror(errno));
		    return NGX_ERROR;
		}
		if ((m_file == nullptr) || (m_file->fd < 0))
		{
//printf("NgxFile set: open fd = %d,  %s\n", (int)m_file->fd, strerror(errno));
			return NGX_ERROR;
		}
		return NGX_OK;
	}

public:

	template<typename T>
	NgxFile(const char* filepath,T* r, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN) 
	{
		set((const u_char*)filepath, r, oflag, mode);
	}

	template<typename T>
	NgxFile(NgxString& filepath, T* r, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN)	
	{
		set(filepath.get(), r, oflag, mode);
	}

	NgxFile()
	{
		m_file = nullptr;
		m_filename = nullptr;
	}

	template<typename T>
	ngx_int_t set(const char* filepath, T* r, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN)
	{
	 	return set((const u_char*)filepath, r, oflag, mode);
	}

	template<typename T>
	ngx_int_t set(NgxString& filepath, T* r, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN) 
	{
		return set(filepath.get(), r, oflag, mode);
	}

	ngx_file_t* get() const
	{
		return m_file;
	}
	const char* getPath() const
	{
		return m_filename;
	}
	size_t getLength() const
	{
		if (m_file == nullptr)
			return 0;
		return m_file->info.st_size;
	}

	template<typename T>
	ngx_int_t cleanup(ngx_pool_cleanup_pt handler, T* r)
	{
		if (m_file == nullptr)
			return 0;
		ngx_pool_cleanup_file_t* clnf = NgxPool(r).alloc<ngx_pool_cleanup_file_t>();
	
		clnf->fd = m_file->fd;
		clnf->name = m_file->name.data;
		clnf->log = r->pool->log;
		if (NgxPool(r).cleanup(handler, clnf) == nullptr)
		{
			return NGX_ERROR;
		}
		return NGX_OK;
	}

};

#endif
