#pragma once

#include "Haze/Scene/Scene.h"
#include "Haze/Program/ProgramAdapter.h"
#include "Haze/Buffers/CubeFramebuffer.h"
#include "Haze/Buffers/GBuffer.h"

namespace Haze 
{

	class Renderer
	{
		public:
			virtual void Draw(Scene* scene, Camera* camera) = 0;
			virtual void Draw(Scene* scene, Camera* camera, Object* object) = 0;
			virtual void Draw(Scene* scene, Camera* camera, Mesh* mesh) = 0;

			void Draw(Mesh* mesh);
			void Clear();

			virtual void OnWindowResizeEvent(WindowResizeEvent& event) {}

		public:
			int _DrawMode = 0;
			int _DrawOverlayMode = 0;
	};

	class ForwardRenderer : public Renderer 
	{
		public:
			ForwardRenderer(ProgramAdapter* program);

			virtual void Draw(Scene* scene, Camera* camera) override;
			virtual void Draw(Scene* scene, Camera* camera, Object* object) override;
			virtual void Draw(Scene* scene, Camera* camera, Mesh* mesh) override;

		private:
			ProgramAdapter* _ForwardPassAdapter;
	};

	class DeferredRenderer : public Renderer
	{
		public:
			DeferredRenderer(glm::vec4 viewport, int shadowResolution, int shadowCount);

			virtual void Draw(Scene* scene, Camera* camera) override;
			virtual void Draw(Scene* scene, Camera* camera, Object* object) override {}
			virtual void Draw(Scene* scene, Camera* camera, Mesh* mesh) override {}

			virtual void OnWindowResizeEvent(WindowResizeEvent& event) override;

		private:
			CubeFramebufferArray _ShadowMapBufferArray;
			GBuffer _GBuffer;

			ProgramAdapter* _GeometryPassAdapter;
			ProgramAdapter* _LightingPassAdapter;
			ProgramAdapter* _ShadingPassAdapter;
	};

}
