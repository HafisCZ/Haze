#pragma once

#include <vector>
#include <tuple>

template <typename T> class enumerate_impl 
{
	public:
		struct enumerable 
		{
			std::size_t index;
			typename T::value_type& value;
		};

		struct iterator 
		{
			iterator(typename T::iterator it, std::size_t index = 0) : _it(it), _index(index) {}

			iterator operator ++ () 
			{ 
				return iterator(++_it, ++_index);
			}

			bool operator != (iterator other) 
			{ 
				return _it != other._it;
			}

			typename T::iterator::value_type item() 
			{
				return *_it;
			}

			enumerable operator*() {
				return enumerable{ _index, *_it }; 
			}

			std::size_t index() {
				return _index;
			}

			private:
				typename T::iterator _it;
				std::size_t _index;
		};

		enumerate_impl(T& t) : t(t) {}

		iterator begin() 
		{
			return iterator(t.begin());
		}

		iterator end() 
		{
			return iterator(t.end());
		}

	private:
		T& t;
};

template <typename T> enumerate_impl<T> enumerate(T& t) 
{
	return enumerate_impl<T>(t);
}