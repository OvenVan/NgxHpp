#include "NgxUnsetValue.hpp"

#ifndef __NGX_VALUE__
#define __NGX_VALUE__

class NgxValue final{
public:
	NgxValue()  = default;
	~NgxValue() = default;
	
	template<typename T>
	static bool invalid(const T& v)
	{
		return v == static_cast<T>(ngx_nil);
	}

	template<typename T, typename U>
	static void init(T& x, const U& u)
	{
		if (invalid(x))
		{
			x = u;
		}
	}

	template<typename T, typename U, typename V>
	static void merge(T& c, U& u, V& v)
	{
		if (invalid(c))
		{
			c = invalid(u) ? v : u;
		}
	}
	
	template<typename T, typename ... Args>
	static void unset(T& v, Args& ... args)
	{
		v = NgxUnsetValue::get();
		unset(args...);
	}

	static void unset(){}
};

#endif
