

#ifndef __NGX_FILTER_HPP__
#define __NGX_FILTER_HPP__

class NgxFilter
{
private:
	template <typename T>
	static void set(T& top, T p)
	{
		next<T>() = top;
		top = p;
	}
	
	template<typename T>
	static T& next()
	{
		static T next_filter;
		return next_filter;
	}

public:
	typedef ngx_http_output_header_filter_pt	header_filter_ptr;
	typedef ngx_http_output_body_filter_pt		body_filter_ptr;

public:
	NgxFilter() = default;
	~NgxFilter() = default;

	static void init(header_filter_ptr header_filter, body_filter_ptr body_filter)
	{
		if (header_filter)
		{
			set(ngx_http_top_header_filter, header_filter);
		}

		if (body_filter)
		{
			set(ngx_http_top_body_filter, body_filter);
		}
	}

	static ngx_int_t next(ngx_http_request_t* r)
	{
		return next<header_filter_ptr>()(r);
	}

	static ngx_int_t next(ngx_http_request_t* r, ngx_chain_t* chain)
	{
		return next<body_filter_ptr>()(r, chain);
	}

}

#endif
