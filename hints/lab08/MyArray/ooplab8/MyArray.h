#pragma once

#include <memory>

template <typename T>
class CMyArray
{
public:

	CMyArray()
		:m_size(0)
	{
	}

	~CMyArray()
	{
	}

	bool IsEmpty()const
	{
		return true;
	}

	size_t GetSize()const
	{
		return m_size;
	}

	T const & operator [] (size_t index)const
	{
		if (index >= m_size)
			throw std::out_of_range("item index is out of range");
		return m_items[index];
	}

	T & operator [] (size_t index)
	{
		if (index >= m_size)
			throw std::out_of_range("item index is out of range");
		return m_items[index];
	}

	void Push(T const & item)
	{
		auto newItems = std::make_unique<T[]>(m_size + 1);
		for (size_t i = 0; i < m_size; ++i)
		{
			newItems[i] = m_items[i];
 		}
		newItems[m_size] = item;
		m_items = std::move(newItems);
		++m_size;
	}

private:
	std::unique_ptr<T[]> m_items;
	size_t m_size;
};

