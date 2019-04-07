#pragma once

#include "Haze/Core.h"
#include "Haze/Program/Program.h"
#include "Haze/Objects/Model.h"
#include "Haze/Layer.h"
#include "Haze/Textures/Texture.h"
#include "Haze/Scene/Camera.h"

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
			Model* _Model = ModelLoader::Load("C:/users/mar21/desktop/table.obj");
			Program* _Program = nullptr;
			Texture* _Texture = TextureLoader::Load("C:/users/mar21/desktop/wood.png");
			Camera* _Camera = new Camera();

			glm::vec3 _CameraPosition = glm::vec3(10.0f, 5.0f, 10.0f);
			glm::vec4 _Color = glm::vec4(1.0f);
	};

}