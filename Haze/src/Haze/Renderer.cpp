#include "hzpch.h"
#include "Renderer.h"

#include "Input.h"
#include "MouseButtonCodes.h"
#include "KeyCodes.h"
#include "Application.h"
#include "Scripts/Interpreter.h"

#include "ImGui/Presets.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"

namespace Haze 
{
	void RendererLayer::OnUpdate()
	{
		if (!ImGui::IsAnyWindowFocused()) {
			static float dy = 0;
			static bool jmp = true;

			if (Input::IsKeyPressed(HZ_KEY_W)) _Camera->Move(0, 0, 1, true);
			if (Input::IsKeyPressed(HZ_KEY_S)) _Camera->Move(0, 0, -1, true);
			if (Input::IsKeyPressed(HZ_KEY_A)) _Camera->Move(-1, 0, 0, true);
			if (Input::IsKeyPressed(HZ_KEY_D)) _Camera->Move(1, 0, 0, true);

			if (Input::IsKeyPressed(HZ_KEY_SPACE) && !jmp) 
			{
				jmp = true;
				dy += 1.0f;
			}

			if (jmp) {
				dy -= 0.1f;
			}

			_Camera->Move(0, dy, 0, true);
			if (_Camera->GetWorldPosition().y < 2.0f) 
			{
				_Camera->_WorldPosition.y = 2.0f;
				_Camera->UpdateMatrices();
				jmp = false;
				dy = 0.0f;
			}

			if (Input::IsKeyPressed(HZ_KEY_1)) {
				/*
				Haze::Model* model = Haze::ModelLoader::Load("");
				if (model) {
					Haze::Object* object = new Haze::Object();
					object->Model = model;

					_Scene->item = object;
				}
				*/
			}
			if (Input::IsKeyPressed(HZ_KEY_0)) _Scene->item = 0;

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
		
		if (_Scene->item) {
			_DefaultProgram->Bind();
			
			glm::mat4 cam = glm::inverse(glm::lookAt(_Camera->GetWorldPosition(), _Camera->GetWorldPosition() + _Camera->GetDirection(), glm::vec3(0.0f, 1.0f, 0.0f)));
			cam = glm::translate(cam, glm::vec3(0.7f, -1.0f, -2.0f));
			cam = glm::rotate(cam, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			cam = glm::scale(cam, glm::vec3(0.4f));

			_DefaultProgram->SetUniform("uModelViewProjectionMatrix", _Camera->GetProjectionMatrix() * _Camera->GetViewMatrix() * cam);
			_DefaultProgram->SetUniform("uColor", 0.0f, 0.0f, 0.0f);

			for (auto m : _Scene->item->Model->Meshes) {
				_DefaultProgram->SetUniform("uTexture[0]", m->Textures[0] != nullptr);
				_DefaultProgram->SetUniform("uTexture[1]", m->Textures[1] != nullptr);
				_DefaultProgram->SetUniform("uTexture[2]", m->Textures[2] != nullptr);

				_DefaultProgram->SetUniform("uTexture0", TextureAllocator::Bind(m->Textures[0]));
				_DefaultProgram->SetUniform("uTexture1", TextureAllocator::Bind(m->Textures[1]));
				_DefaultProgram->SetUniform("uTexture2", TextureAllocator::Bind(m->Textures[2]));

				Draw(m);
			}
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
		static bool win_camera = false;
		static bool win_repository = false;
		static bool win_setskybox = false;
		static bool win_insertobject = false;
		static bool win_objectmanager = false;
		static bool win_lightmanager = false;
		static bool win_script = false;
		
		static std::array<char, 1000> script;
		static bool script_execute = false;

		if (win_camera) GUI::CameraWindow(win_camera, _Camera);
		if (win_repository) GUI::RepositoryWindow(win_repository);
		if (win_script) GUI::ScriptWindow(win_script, script, script_execute);
		if (win_setskybox) GUI::SetSkyboxWindow(win_setskybox, _Scene);
		if (win_insertobject) GUI::InsertObjectWindow(win_insertobject, _Scene);
		if (win_objectmanager) GUI::ObjectManagerWindow(win_objectmanager, win_insertobject, _Scene, _Camera);
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
					ImGui::RadioButton("Shadows", &_Mode, 5);
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
			ImGui::MenuItem("Script", 0, &win_script);
			ImGui::EndMainMenuBar();

			if (script_execute) 
			{
				Interpreter::ExecuteScript(script.data(), _Scene, _Camera);
				script_execute = false;
			}
		}
	}

}