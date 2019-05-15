#pragma once

#include "Haze/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Haze {

	class OpenGLContext : public GraphicsContext {
		public:
			OpenGLContext(GLFWwindow* windowHandle);

			virtual void Init() override;
			virtual void SwapBuffers() override;

		private:
			GLFWwindow* _WindowHandle;
	};

}