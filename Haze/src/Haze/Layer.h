#pragma once

#include "Haze/Core.h"
#include "Haze/Events/Event.h"

namespace Haze 
{

	class HAZE_API Layer 
	{
		public:
			Layer(const std::string& debugName = "Layer");
			virtual ~Layer();

			virtual void OnAttach() { }
			virtual void OnDetach() { }
			virtual void OnUpdate() { }
			virtual void OnEvent(Event& event) { }

			inline const std::string& GetName() const { return _DebugName; }

		protected:
			std::string _DebugName;
	};

}