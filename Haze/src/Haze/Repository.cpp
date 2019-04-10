#include "hzpch.h"
#include "Repository.h"

namespace Haze 
{

	Repository Repository::_Instance;

	void* Repository::RequestImpl(const std::string& id, std::function<void*()>& value_gen) 
	{
		auto it = _Pointers.lower_bound(id);
		if (it == std::end(_Pointers))
		{
			it = _Pointers.insert(it, std::make_pair(id, value_gen()));
		}

		return it->second;
	}

	void* Repository::SeekImpl(const std::string& id) 
	{
		auto it = _Pointers.lower_bound(id);
		if (it == _Pointers.end()) 
		{
			return nullptr;
		} 
		else
		{
			return it->second;
		}
	}

}