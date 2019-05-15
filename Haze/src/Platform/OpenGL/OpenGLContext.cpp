#include "hzpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Haze {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : _WindowHandle(windowHandle) {

	}

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize GLAD!");

		glEnable(GL_MULTISAMPLE);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(_WindowHandle);
	}

}