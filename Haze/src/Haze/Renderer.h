#pragma once

#include "Haze/Core.h"
#include "Haze/Program/Program.h"
#include "Haze/Objects/Model.h"
#include "Haze/Layer.h"
#include "Haze/Textures/Texture.h"
#include "Haze/Scene/Camera.h"
#include "Haze/Scene/Scene.h"

namespace Haze 
{
	
	class HAZE_API RendererLayer : public Layer
	{
		public:
			RendererLayer() : Layer("Render Layer"), _Program(new Program("../shaders/default", 3)) { }

			void OnUpdate() override;
			void OnImGuiRender() override;

			void OnEvent(Event& event) override { }

		private:
			Program* _Program = nullptr;

			Camera* _Camera = new Camera();
			Scene* _Scene = new Scene();
	};

}