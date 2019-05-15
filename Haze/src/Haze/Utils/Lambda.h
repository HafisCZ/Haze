#pragma once

template <typename CT, typename ... A> struct function : public function<decltype(&CT::operator())(A ...)> {};

template <typename C> struct function<C> {
	function(const C& obj) : _object(obj) {}
	
	template <typename ... Args> typename std::result_of<C(Args ...)>::type operator()(Args ... a) { 
		return this->_object.operator()(a ...); 
	}

	template <typename ... Args> typename std::result_of<const C(Args ...)>::type operator()(Args ... a) const { 
		return this->_object.operator()(a ...);
	}

	private:
		C _object;
};

template <typename C> auto Lambda(const C& obj) {
	return function<C>(obj); 
}