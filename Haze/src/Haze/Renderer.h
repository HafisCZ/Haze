#pragma once

#include "Haze/Core.h"
#include "Haze/Layer.h"
#include "Haze/Buffers/CubeFramebuffer.h"
#include "Haze/Program/ProgramAdapter.h"
#include "Haze/Buffers/GBuffer.h"

namespace Haze 
{
	
	class HAZE_API RendererLayer : public Layer
	{
		public:
			RendererLayer() : 
				Layer("Render Layer"), 
				_ShadingBuffer(2048, 5), 
				_Buffer(1280, 720),
				_GeometryAdapter(new GeometryPassAdapter(new Program("../shaders/geom", ShaderTypeFragment | ShaderTypeVertex))),
				_ShadingAdapter(new ShadingPassAdapter(new Program("../shaders/shade", ShaderTypeGeometry | ShaderTypeFragment | ShaderTypeVertex))),
				_LightingAdapter(new LightPassAdapter(new Program("../shaders/light", ShaderTypeFragment | ShaderTypeVertex))),
				_SkyboxAdapter(new SkyboxAdapter(new Program("../shaders/skybox", ShaderTypeFragment | ShaderTypeVertex)))
			{ 
				Program* prog = _LightingAdapter->GetProgram();
				
				prog->Bind();
				prog->SetUniform("uPosition", 0);
				prog->SetUniform("uNormal", 1);
				prog->SetUniform("uAlbedoSpecular", 2);
				prog->SetUniform("uPointShadowMap", 3);
			}

			void OnUpdate() override;
			void OnImGuiRender() override;
			
			void OnEvent(Event& event) override;

		private:
			void Draw(Mesh* mesh);
			void Draw();
			void DrawQuad();

		private:
			GBuffer _Buffer;
			CubeFramebufferArray _ShadingBuffer;

			int _Mode = 0;
			
			ProgramAdapter* _GeometryAdapter;
			ProgramAdapter* _ShadingAdapter;
			ProgramAdapter* _LightingAdapter;
			ProgramAdapter* _SkyboxAdapter;

			Camera* _Camera = new Camera();
			Scene* _Scene = new Scene();
	};

}