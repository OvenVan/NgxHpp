
#ifndef __NGX_UNSET_VALUE_HPP__
#define __NGX_UNSET_VALUE_HPP__

class NgxUnsetValue final
{
public:
	template <typename T>
	operator T() const
	{
		return static_cast<T>(-1);
	}

	template <typename T>
	operator T*() const
	{
		return reinterpret_cast<T*>(-1);
	}

	static const NgxUnsetValue& get()
	{
		static NgxUnsetValue const v = {};
		return v;
	}
	
};

/*__attribute__((unused))*/ static auto&& ngx_nil = NgxUnsetValue::get();

#endif
