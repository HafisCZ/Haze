#include "hzpch.h"
#include "Renderer.h"

#include "Program/ProgramAdapter.h"
#include "Input.h"
#include "MouseButtonCodes.h"
#include "KeyCodes.h"
#include "Application.h"

#include "ImGui/Presets.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"

#define UUID3(name, i, j, k)	std::string(name"##uuid" + std::to_string(i * 100000 + j * 1000 + k)).c_str()
#define UUID2(name, i, j)		std::string(name"##uuid" + std::to_string(i * 100000 + j * 1000)).c_str()
#define UUID1(name, i)			std::string(name"##uuid" + std::to_string(i * 100000)).c_str()
#define UUID(name, i, j, k)		std::string(name + "##uuid" + std::to_string(i * 100000 + j * 1000 + k)).c_str()

namespace Haze 
{
	void RendererLayer::OnUpdate()
	{
		if (!ImGui::IsAnyWindowFocused()) {
			if (Input::IsKeyPressed(HZ_KEY_W)) _Camera->Move(0, 0, 1);
			if (Input::IsKeyPressed(HZ_KEY_S)) _Camera->Move(0, 0, -1);
			if (Input::IsKeyPressed(HZ_KEY_A)) _Camera->Move(-1, 0, 0);
			if (Input::IsKeyPressed(HZ_KEY_D)) _Camera->Move(1, 0, 0);

			if (Input::IsKeyPressed(HZ_KEY_SPACE)) _Camera->Move(0, 1, 0);
			if (Input::IsKeyPressed(HZ_KEY_C)) _Camera->Move(0, -1, 0);

			static bool lookMode = false;
			static bool lookModeLast = false;
			bool lookModeNow = Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_2);

			if (!lookModeLast && lookModeNow) {
				if (lookMode) {
					glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					lookMode = false;
				} else {
					glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					lookMode = true;
				}
			}

			lookModeLast = lookModeNow;

			if (lookMode) {
				auto[x, y] = Input::GetMousePosition();
				static float lastx = x, lasty = y;
				
				if (lookModeLast) {
					lastx = x;
					lasty = y;
				}

				float xo = x - lastx;
				float yo = lasty - y;

				_Camera->Look(xo, yo);

				lastx = x;
				lasty = y;
			}
		}

		Draw();
	}

	void RendererLayer::OnEvent(Event& event) {
		if (event.GetEventType() == EventType::WindowResize) {
			WindowResizeEvent& we = (WindowResizeEvent&) event;

			glViewport(0, 0, we.GetWidth(), we.GetHeight());

			_Camera->OnWindowResizeEvent(we);
			_Buffer.Resize(we.GetWidth(), we.GetHeight());
		}
	}

	void RendererLayer::Draw(Mesh* mesh)
	{
		mesh->VAO->Bind();	
		mesh->IBO->Bind();
		glDrawElements(GL_TRIANGLES, (int) mesh->Triangles.size() * 3, GL_UNSIGNED_INT, nullptr);

		TextureAllocator::Flush();
	}

	void RendererLayer::Draw() 
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_Buffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_GeometryAdapter->Bind();
		_GeometryAdapter->Set(_Scene, _Camera);
		for (auto o : _Scene->Objects) 
		{
			_GeometryAdapter->Set(o, _Camera);
			for (auto m : o->Model->Meshes)
			{
				_GeometryAdapter->Set(m, _Camera);
				Draw(m);
			}
		}

		_Buffer.Unbind();

		if ([this]() -> bool { for (auto l : _Scene->Lights->Point) if (l->IsCastingShadow()) return true; return false; }()) 
		{
			glDisable(GL_CULL_FACE);
			_ShadingBuffer.Bind();

			_ShadingAdapter->Bind();
			_ShadingAdapter->Set(_Scene, _Camera);
			for (auto o : _Scene->Objects) 
			{
				_ShadingAdapter->Set(o, _Camera);
				for (auto m : o->Model->Meshes) 
				{
					Draw(m);
				}
			}

			_ShadingBuffer.Unbind(3);
			glEnable(GL_CULL_FACE);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_LightingAdapter->Bind();

		_Buffer.BindTextures(0);

		_LightingAdapter->Set(_Scene, _Camera);
		_LightingAdapter->GetProgram()->SetUniform("uDrawMode", _Mode);

		DrawQuad();

		_Buffer.Copy();

		// OPT FORWARD STEP HERE

		if (_Scene->Skybox) {
			static unsigned int vao = []() -> unsigned int {
				static float vertices[] = {
					-1.0f,  1.0f, -1.0f,
					-1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,
					 1.0f,  1.0f, -1.0f,
					-1.0f,  1.0f, -1.0f,

					-1.0f, -1.0f,  1.0f,
					-1.0f, -1.0f, -1.0f,
					-1.0f,  1.0f, -1.0f,
					-1.0f,  1.0f, -1.0f,
					-1.0f,  1.0f,  1.0f,
					-1.0f, -1.0f,  1.0f,

					 1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,

					-1.0f, -1.0f,  1.0f,
					-1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f, -1.0f,  1.0f,
					-1.0f, -1.0f,  1.0f,

					-1.0f,  1.0f, -1.0f,
					 1.0f,  1.0f, -1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					-1.0f,  1.0f,  1.0f,
					-1.0f,  1.0f, -1.0f,

					-1.0f, -1.0f, -1.0f,
					-1.0f, -1.0f,  1.0f,
					 1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,
					-1.0f, -1.0f,  1.0f,
					 1.0f, -1.0f,  1.0f
				};

				unsigned int vao, vbo;
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
				return vao;
			}();

			glDepthFunc(GL_LEQUAL);

			_SkyboxAdapter->Bind();
			_SkyboxAdapter->Set(_Scene, _Camera);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			TextureAllocator::Flush();

			glDepthFunc(GL_LESS);
		}
	}

	void RendererLayer::DrawQuad() 
	{
		static float vertices[] = { -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
		static unsigned int vao = []() -> unsigned int {
			unsigned int vao, vbo;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			return vao;
		}();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void RendererLayer::OnImGuiRender()
	{
		/*
			New menu system
		*/
		static bool win_camera = false;
		static bool win_repository = false;
		static bool win_setskybox = false;
		static bool win_insertobject = false;
		static bool win_objectmanager = false;
		static bool win_lightmanager = false;

		if (win_camera) GUI::CameraWindow(win_camera, _Camera);
		if (win_repository) GUI::RepositoryWindow(win_repository);
		if (win_setskybox) GUI::SetSkyboxWindow(win_setskybox, _Scene);
		if (win_insertobject) GUI::InsertObjectWindow(win_insertobject, _Scene);
		if (win_objectmanager) GUI::ObjectManagerWindow(win_objectmanager, win_insertobject, _Scene);
		if (win_lightmanager) GUI::LightManagerWindow(win_lightmanager, _Scene, _Camera);

		if (ImGui::BeginMainMenuBar()) 			
		{
			if (ImGui::BeginMenu("HAZE")) 
			{
				double frametime = GUI::Sample();
				if (frametime < 0.018) ImGui::TextColored(ImVec4(RGBTOF(23, 91, 37)), "%F", frametime);
				else if (frametime < 0.021) ImGui::TextColored(ImVec4(RGBTOF(211, 214, 23)), "%F", frametime);
				else ImGui::TextColored(ImVec4(RGBTOF(214, 64, 23)), "%F", frametime);

				GUI::BigSeparator();

				if (ImGui::BeginMenu("Drawing mode")) 
				{
					ImGui::RadioButton("Default", &_Mode, 0);
					GUI::BigSpace();
					ImGui::RadioButton("Position", &_Mode, 1);
					ImGui::RadioButton("Normals", &_Mode, 2);
					GUI::BigSeparator();		
					ImGui::Text("Textures");
					GUI::BigSpace();
					ImGui::RadioButton("Diffuse", &_Mode, 3);
					ImGui::RadioButton("Specular", &_Mode, 4);
					ImGui::EndMenu();
				}

				GUI::BigSeparator();

				ImGui::MenuItem("Repository", 0, &win_repository);
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Scene")) 
			{
				ImGui::MenuItem("Objects", 0, &win_objectmanager);
				GUI::BigSpace();
				ImGui::MenuItem("Lights", 0, &win_lightmanager);
				GUI::BigSeparator();
				ImGui::MenuItem("Skybox", 0, &win_setskybox);

				ImGui::EndMenu();
			}

			ImGui::MenuItem("Camera", 0, &win_camera);

			ImGui::EndMainMenuBar();
		}
	}

}