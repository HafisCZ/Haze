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
			inline static void* Request(const std::string& id, std::function<void*()> value_gen) { return _Instance.RequestImpl(id, value_gen); }
			inline static void* Seek(const std::string& id) { return _Instance.SeekImpl(id); }

			void* RequestImpl(const std::string& id, std::function<void*()>& value_gen);
			void* SeekImpl(const std::string& id);

		private:
			static Repository _Instance;

			std::unordered_map<std::string, void*> _Pointers;
	};

}