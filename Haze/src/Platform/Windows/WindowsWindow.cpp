#include "hzpch.h"
#include "WindowsWindow.h"

#include "Haze/Events/MouseEvent.h"
#include "Haze/Events/ApplicationEvent.h"
#include "Haze/Events/KeyEvent.h"

#include <glad/glad.h>

namespace Haze 
{

	static bool _GLFWInitialized = false;
	
	static void GLFWErrorCallback(int error, const char* description)
	{
		HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props) 
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) 
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow() 
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		_Data.Title = props.Title;
		_Data.Width = props.Width;
		_Data.Height = props.Height;

		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (!_GLFWInitialized) 
		{
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			_GLFWInitialized = true;
		}

		_Window = glfwCreateWindow((int)props.Width, (int)props.Height, _Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_Window);

		int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize GLAD!");

		glfwSetWindowUserPointer(_Window, &_Data);
		
		SetVSync(true);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// GLFW callbacks
		glfwSetWindowSizeCallback(_Window, [](GLFWwindow* window, int width, int height) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(_Window, [](GLFWwindow* window) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) 
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(_Window, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(_Window, [](GLFWwindow* window, int button, int action, int mods) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}

				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(_Window, [](GLFWwindow* window, double xOffset, double yOffset) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float) xOffset, (float) yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(_Window, [](GLFWwindow* window, double x, double y) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float) x, (float) y);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown() 
	{
		glfwDestroyWindow(_Window);
	}

	void WindowsWindow::OnUpdate() 
	{
		glfwPollEvents();
		glfwSwapBuffers(_Window);
	}

	void WindowsWindow::SetHideCursor(bool enabled)
	{
		if (enabled) 
		{
			glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);		
		} 
		else 
		{
			glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	void WindowsWindow::SetVSync(bool enabled) 
	{
		if (enabled) 
		{
			glfwSwapInterval(1);
		} 
		else 
		{
			glfwSwapInterval(0);
		}

		_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return _Data.VSync;
	}

}