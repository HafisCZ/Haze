#pragma once

#include "Haze/Core.h"
#include "Haze/ImGui/Presets.h"

namespace lambda
{

	template <typename CT, typename ... A> struct function : public function<decltype(&CT::operator())(A ...)> {};
	template <typename C> struct function<C> 
	{
		function(const C& obj) : _object(obj) {}
		template <typename ... Args> typename std::result_of<C(Args ...)>::type operator()(Args ... a) { return this->_object.operator()(a ...); }
		template <typename ... Args> typename std::result_of<const C(Args ...)>::type operator()(Args ... a) const { return this->_object.operator()(a ...); }

		private:
			C _object;
	};

	template <typename C> auto make(const C& obj) { return function<C>(obj); }

}

namespace Haze 
{
	class HAZE_API Repository 
	{
		friend struct GUI;

		public:
			inline static void* Load(const std::string& id, std::function<void*()> gen) { return _Instance.LoadImpl(id, gen); }
			inline static void Unload(void* ptr) { _Instance.UnloadImpl(ptr); }

			void* LoadImpl(const std::string& id, std::function<void*()>& gen);
			void UnloadImpl(void* ptr);

		private:
			static Repository _Instance;

			std::unordered_map<std::string, void*> _PtrNameMap;
			std::unordered_map<void*, int> _PtrCallMap;
	};

}