#include "hzpch.h"
#include "Repository.h"

namespace Haze 
{

	Repository Repository::_Instance;

	void* Repository::LoadImpl(const std::string& id, std::function<void*()>& gen)
	{
		auto iterator = _PtrNameMap.lower_bound(id);
		if (iterator == std::end(_PtrNameMap))
		{
			iterator = _PtrNameMap.insert(iterator, std::make_pair(id, gen()));
			_PtrCallMap.insert(_PtrCallMap.end(), std::make_pair(iterator->second, 0));
		}

		_PtrCallMap[iterator->second]++;

		return iterator->second;
	}

	void Repository::UnloadImpl(void* ptr)
	{
		auto iterator = _PtrCallMap.lower_bound(ptr);
		if (iterator != std::end(_PtrCallMap)) 
		{
			iterator->second--;

			if (iterator->second <= 0) 
			{
				for (auto it = _PtrNameMap.begin(); it != _PtrNameMap.end(); it++) {
					if (it->second == ptr) {
						_PtrNameMap.erase(it);
						break;
					}
				}

				delete iterator->first;

				_PtrCallMap.erase(iterator);
			}
		}
	}

}