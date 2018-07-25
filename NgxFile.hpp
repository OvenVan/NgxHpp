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
	void set(T* r, const u_char* filepath, ngx_int_t oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, ngx_int_t mode = NGX_FILE_OPEN, ngx_int_t access = 0)
	{
		m_file = NgxPool(r->pool).alloc<ngx_file_t>();
		m_file->fd = ngx_open_file(filepath, oflag, NGX_FILE_OPEN, access);
printf("NgxFile cons : open fd = %d,  %s\n", (int)m_file->fd, strerror(errno));
		m_file->name.data = (u_char*)filepath;
		m_file->name.len = strlen((const char*)filepath);
		m_file->log = r->connection->log;
		m_filename = toStr(m_file->name, r);
	}

public:

	template<typename T>
	NgxFile(T* r, const char* filepath, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN) 
	{
		set(r, (const u_char*)filepath, oflag, mode);
	}

	template<typename T>
	NgxFile(T* r, NgxString& filepath, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN)	
	{
		set(r, filepath.get(), oflag, mode);
	}

	NgxFile()
	{
		m_file = nullptr;
		m_filename = nullptr;
	}

	template<typename T>
	ngx_int_t set(T* r, const char* filepath, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN)
	{
	 	set(r, (const u_char*)filepath, oflag, mode);

		if ((m_file == nullptr) || (m_file->fd < 0))
		{
			return NGX_ERROR;
		}
		return NGX_OK;
	}

	template<typename T>
	ngx_int_t set(T* r, NgxString& filepath, int oflag = NGX_FILE_RDONLY | NGX_FILE_NONBLOCK, int mode = NGX_FILE_OPEN) 
	{
		set(r, filepath.get(), oflag, mode);
printf("ngxfile: file = %p\n", m_file);
printf("ngxfile: fd = %d\n", m_file->fd);

		if ((m_file == nullptr) || (m_file->fd < 0))
		{
			return NGX_ERROR;
		}
		return NGX_OK;

	}



	ngx_file_t* get()
	{
		return m_file;
	}

	const char* getPath()
	{
		return m_filename;
	}
	ngx_int_t getLength()
	{
		if (m_file == nullptr)
			return 0;
		return m_file->info.st_size;
	}

	template<typename T>
	ngx_int_t cleanup(T* r, ngx_pool_cleanup_pt handler)
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
