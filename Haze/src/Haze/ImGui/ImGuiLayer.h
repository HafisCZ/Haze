#pragma once

#include "Haze/Layer.h"

#include "Haze/Events/ApplicationEvent.h"
#include "Haze/Events/KeyEvent.h"
#include "Haze/Events/MouseEvent.h"

namespace Haze 
{

	class HAZE_API ImGuiLayer : public Layer
	{
		public:
			ImGuiLayer();
			~ImGuiLayer();

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			virtual void OnImGuiRender() override;

			void Begin();
			void End();
	};

}