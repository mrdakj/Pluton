#ifndef OPTIONAL_REF_HPP
#define OPTIONAL_REF_HPP 

template <class T>
class optional_ref {
private:
	T* m_data;

public:
	optional_ref(T& data)
		: m_data(&data)
	{}

	optional_ref()
		: m_data(nullptr)
	{}

	bool empty() const
	{
		return m_data == nullptr;
	}

	T get() const
	{
		if (m_data != nullptr)
			return *m_data;

		return T();
	}

	operator bool() const
	{ 
		return m_data != nullptr;
	}
};

#endif /* OPTIONAL_REF_HPP */
