#pragma once

#include "Haze/Core.h"
#include "Haze/ImGui/Presets.h"

namespace Haze 
{
	class Repository 
	{
		friend struct GUI;

		public:
			inline static void* Load(const std::string& id, std::function<void*()> gen) { return _Instance.LoadImpl(id, gen); }
			void* LoadImpl(const std::string& id, std::function<void*()>& gen);

			template <typename T> inline static void Unload(void* ptr) { _Instance.UnloadImpl<T>(ptr); }
			template <typename T> void UnloadImpl(void* ptr) {
				if (T* cptr = static_cast<T*>(ptr)) {
					auto iterator = _PtrCallMap.find(ptr);
					if (iterator != _PtrCallMap.end()) {
						iterator->second--;
						if (iterator->second <= 0) {
							for (auto it = _PtrNameMap.begin(); it != _PtrNameMap.end(); it++) {
								_PtrNameMap.erase(it);
								break;
							}

							delete cptr;
							_PtrCallMap.erase(iterator);
						}
					}
				}
			}

		private:
			static Repository _Instance;

			std::unordered_map<std::string, void*> _PtrNameMap;
			std::unordered_map<void*, int> _PtrCallMap;
	};

}