#include "hzpch.h"
#include "Repository.h"

namespace Haze 
{

	Repository Repository::_Instance;

	void* Repository::LoadImpl(const std::string& id, std::function<void*()>& gen)
	{
		auto iterator = _PtrNameMap.find(id);
		if (iterator == std::end(_PtrNameMap))
		{
			iterator = _PtrNameMap.insert(iterator, std::make_pair(id, gen()));
			_PtrCallMap.insert(_PtrCallMap.end(), std::make_pair(iterator->second, 0));
		}

		_PtrCallMap[iterator->second]++;

		return iterator->second;
	}

}