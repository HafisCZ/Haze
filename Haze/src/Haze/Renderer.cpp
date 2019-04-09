#include "hzpch.h"
#include "Renderer.h"

#include "Program/ProgramAdapter.h"
#include "Input.h"
#include "MouseButtonCodes.h"
#include "KeyCodes.h"
#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "STB/stb_image.h"

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
		static double lastTime = 0;
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;

		static auto load_object = [this](const std::string& path) {
			Model* model = ModelLoader::Load(path);
			if (model) {
				Object* object = new Object();
				object->Model = model;
				_Scene->Objects.push_back(object);
			}
		};

		static bool win_load_skybox_show = false;
		static auto win_load_skybox = [this]() {
			ImGui::Begin("Set skybox", &win_load_skybox_show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize("Add new object", ImVec2(400, 50));

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "File");

			ImGui::PushItemWidth(350);
			static std::array<char, 200> buffer;
			if (ImGui::InputText("##skyboxfile", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
				if (_Scene->Skybox) delete _Scene->Skybox;
				_Scene->Skybox = (TextureCube*) TextureLoader::LoadCube(std::string(buffer.data()));

				win_load_skybox_show = false;
			}

			ImGui::End();
		};

		static bool win_load_object_show = false;
		static auto win_load_object = [this]() {
			ImGui::Begin("Add new object", &win_load_object_show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize("Add new object", ImVec2(400, 150));

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Loader flags");
			static bool win_load_object_flags[] = { true, true, true, true };

			ImGui::Columns(2, 0, false);
			ImGui::Checkbox("Triangulate", &win_load_object_flags[0]);
			ImGui::Checkbox("Generate tangents", &win_load_object_flags[1]);
			ImGui::NextColumn();
			ImGui::Checkbox("Smooth normals", &win_load_object_flags[2]);
			ImGui::Checkbox("Flip UVs", &win_load_object_flags[3]);
			ImGui::Columns(1); 

			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "File");

			ImGui::PushItemWidth(350);
			static std::array<char, 200> buffer;
			if (ImGui::InputText(".obj", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
				load_object(std::string(buffer.data()) + ".obj");
				win_load_object_show = false;
			}


			ImGui::End();
		};

		static bool win_list_object_show = false;
		static auto win_list_object = [this]() {
			ImGui::Begin("Objects", &win_list_object_show, ImGuiWindowFlags_NoCollapse);

			for (unsigned int i = 0; i < _Scene->Objects.size(); i++) 
			{
				auto object = _Scene->Objects[i];
				auto model = object->Model;
				auto& matrix = object->Matrix;

				bool headerExpanded = ImGui::CollapsingHeader(UUID("Object " + std::to_string(i), i, 0, 0), ImGuiTreeNodeFlags_AllowOverlapMode);
				ImGui::SameLine(ImGui::GetWindowWidth() - 55);
				if (ImGui::Button(UUID1("Remove", i))) {
					_Scene->Objects.erase(_Scene->Objects.begin() + i);
					delete object;
					break;
				}

				if (headerExpanded)
				{
					if (ImGui::TreeNode(UUID1("Meshes", i))) 
					{
						for (unsigned int j = 0; j < model->Meshes.size(); j++) {
							if (ImGui::TreeNode(UUID(std::to_string(j), i, j, 0))) {
								ImGui::Text("Vertices");
								ImGui::SameLine(ImGui::GetWindowWidth() - 50);
								ImGui::Text("%d", model->Meshes[j]->Vertices.size());

								ImGui::Text("Triangles");
								ImGui::SameLine(ImGui::GetWindowWidth() - 50);
								ImGui::Text("%d", model->Meshes[j]->Triangles.size());

								if (ImGui::TreeNode(UUID2("Textures", i, j))) 
								{
									static std::string textureNames[] = { "Diffuse", "Normal", "Specular" };

									for (unsigned int k = 0; k < 3; k++) 
									{
										if (model->Meshes[j]->Textures[k]) 
										{
											if (ImGui::TreeNode(UUID(textureNames[k], i, j, k)))
											{
												auto texture = model->Meshes[j]->Textures[k];

												ImGui::Text("Width");
												ImGui::SameLine(ImGui::GetWindowWidth() - 50);
												ImGui::Text("%d", texture->_Width);

												ImGui::Text("Height");
												ImGui::SameLine(ImGui::GetWindowWidth() - 50);
												ImGui::Text("%d", texture->_Height);

												ImGui::Text("Channels");
												ImGui::SameLine(ImGui::GetWindowWidth() - 50);
												ImGui::Text("%d", texture->_PPM);

												ImGui::Text("Binds");
												ImGui::SameLine(ImGui::GetWindowWidth() - 50);
												ImGui::Text("%d", texture->_TotalBinds);

												ImGui::TreePop();
											}
										}
									}
									ImGui::TreePop();
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}

					if (ImGui::TreeNode(UUID1("Matrix", i))) 
					{
						if (ImGui::InputFloat3("Position", &matrix._Position.x))  matrix.UpdateMatrices(true);
						if (ImGui::SliderFloat3("Scale", &matrix._Scale.x, 0.1f, 10.0f, "%.1f"))  matrix.UpdateMatrices(true);
						if (ImGui::SliderFloat3("Rotation", &matrix._Rotation.x, -3.14f, 3.14f, "%.2f"))  matrix.UpdateMatrices(true);
					
						if (ImGui::Button(UUID1("Remove", i))) {
							matrix._Position = glm::vec3(0.0f);
							matrix._Scale = glm::vec3(1.0f);
							matrix._Rotation = glm::vec3(0.0f);
							matrix.UpdateMatrices(true);
						}

						ImGui::TreePop();
					}
				}
			}

			ImGui::End();
		};

		static bool win_camera_show = false;
		static auto win_camera = [this]() {
			ImGui::Begin("Camera", &win_camera_show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize("Camera", ImVec2(450, 250));

			if (ImGui::InputFloat3("Position", &_Camera->_WorldPosition[0])) _Camera->UpdateMatrices();
			if (ImGui::SliderFloat("Yaw", &_Camera->_Yaw, -360.0f, 360.0f, "%.1f")) _Camera->UpdateMatrices();
			if (ImGui::SliderFloat("Pitch", &_Camera->_Pitch, -89.0f, 89.0f, "%.1f")) _Camera->UpdateMatrices();

			ImGui::Separator();
			
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Move using local axis");
			ImGui::Spacing();

			ImGui::Text("X Axis");
			ImGui::SameLine();
			if (ImGui::Button("+##0")) { _Camera->Move(+1.0f, 0.0f, 0.0f); _Camera->UpdateMatrices(); }
			ImGui::SameLine();
			if (ImGui::Button("-##0")) { _Camera->Move(-1.0f, 0.0f, 0.0f); _Camera->UpdateMatrices(); }		

			ImGui::Text("Y Axis");
			ImGui::SameLine();
			if (ImGui::Button("+##1")) { _Camera->Move(0.0f, +1.0f, 0.0f); _Camera->UpdateMatrices(); }
			ImGui::SameLine();
			if (ImGui::Button("-##1")) { _Camera->Move(0.0f, -1.0f, 0.0f); _Camera->UpdateMatrices(); }

			ImGui::Text("Z Axis");
			ImGui::SameLine();
			if (ImGui::Button("+##2")) { _Camera->Move(0.0f, 0.0f, +1.0f); _Camera->UpdateMatrices(); }
			ImGui::SameLine();
			if (ImGui::Button("-##2")) { _Camera->Move(0.0f, 0.0f, -1.0f); _Camera->UpdateMatrices(); }

			ImGui::Separator();
			ImGui::InputFloat("camera speed", &_Camera->_MoveSpeed);
			ImGui::InputFloat("mouse sensitivity", &_Camera->_LookSpeed);

			ImGui::End();
		};

		static bool win_list_light_show = false;
		static auto win_list_light = [this]() {
			ImGui::Begin("Lights", &win_list_light_show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

			if (ImGui::BeginMenuBar()) 
			{
				if (ImGui::BeginMenu("Add point light")) {
					if (ImGui::MenuItem("Default")) _Scene->Lights->Point.push_back(new PointLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.007f, 0.0002f, false));
					if (ImGui::MenuItem("At camera")) _Scene->Lights->Point.push_back(new PointLight(glm::vec3(1.0f), _Camera->GetWorldPosition(), 0.0f, 0.0f, 0.007f, 0.0002f, false));

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			if (ImGui::CollapsingHeader("Ambient light")) 
			{
				ImGui::ColorEdit3("Color##ambient", &_Scene->Lights->Ambient->GetData()[0].x);
				ImGui::Spacing();
				ImGui::SliderFloat("Intensity##ambient", &_Scene->Lights->Ambient->GetData()[2].x, 0.0f, 1.0f);
			}

			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Vector light")) 
			{
				ImGui::ColorEdit3("Color##vector", &_Scene->Lights->Vector->GetData()[0].x);
				ImGui::Spacing();
				ImGui::InputFloat3("Direction##vector", &_Scene->Lights->Vector->GetData()[1].x);
				ImGui::Spacing();
				ImGui::SliderFloat("Intensity##vector", &_Scene->Lights->Vector->GetData()[2].y, 0.0f, 1.0f);
				ImGui::SliderFloat("Specular##vector", &_Scene->Lights->Vector->GetData()[2].z, 0.0f, 1.0f);
				ImGui::Separator();
				if (ImGui::Button("Set using camera")) {
					_Scene->Lights->Vector->GetData()[1] = _Camera->GetDirection();
				}
			}

			ImGui::Spacing();

			for (unsigned int i = 0; i < _Scene->Lights->Point.size(); i++) 
			{
				bool headerExpanded = ImGui::CollapsingHeader(UUID("Point light " + std::to_string(i), i, 0, 0), ImGuiTreeNodeFlags_AllowOverlapMode);
				ImGui::SameLine(ImGui::GetWindowWidth() - 55);
				if (ImGui::Button(UUID1("Remove", i))) 
				{
					auto l = _Scene->Lights->Point[i];
					_Scene->Lights->Point.erase(_Scene->Lights->Point.begin() + i);
					delete l;
					break;
				}

				if (headerExpanded) 
				{
					auto& light = _Scene->Lights->Point[i]->GetData();

					ImGui::ColorEdit3(UUID1("Color", i), &light[0].x);

					ImGui::Spacing();

					ImGui::InputFloat3(UUID1("Position", i), &light[1].x);

					ImGui::Spacing();

					ImGui::SliderFloat(UUID1("Intensity", i), &light[2].y, 0.0f, 1.0f);
					ImGui::SliderFloat(UUID1("Specular", i), &light[2].z, 0.0f, 1.0f);

					ImGui::Spacing();

					ImGui::InputFloat(UUID1("Linear attenuation", i), &light[3].y);
					ImGui::InputFloat(UUID1("Quad attenuation", i), &light[3].z, 0.0f, 0.0f, "%.4f");

					ImGui::Spacing();

					ImGui::Checkbox(UUID1("Casts shadow", i), &_Scene->Lights->Point[i]->IsCastingShadow());
					ImGui::Separator();
					if (ImGui::Button("Set using camera")) {
						_Scene->Lights->Point[i]->GetData()[1] = _Camera->GetWorldPosition();
					}
				}

				ImGui::Spacing();
			}

			ImGui::End();
		};

		if (win_load_object_show) win_load_object();
		if (win_list_object_show) win_list_object();
		if (win_list_light_show) win_list_light();
		if (win_load_skybox_show) win_load_skybox();
		if (win_camera_show) win_camera();

		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu(std::to_string(frameTime).c_str())) {
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Draw modes"))
			{
				ImGui::RadioButton("Default", &_Mode, 0);

				ImGui::Separator();

				ImGui::RadioButton("Position", &_Mode, 1);
				ImGui::RadioButton("Normals", &_Mode, 2);

				ImGui::Separator();
				ImGui::Text("Textures");
				ImGui::Spacing();

				ImGui::RadioButton("Diffuse", &_Mode, 3);
				ImGui::RadioButton("Specular", &_Mode, 4);

				ImGui::EndMenu();
			}

			ImGui::Separator();

			ImGui::MenuItem("Objects", NULL, &win_list_object_show);
			ImGui::MenuItem("+##0", NULL, &win_load_object_show);

			ImGui::Separator();

			ImGui::MenuItem("Lights", NULL, &win_list_light_show);

			ImGui::Separator();

			ImGui::MenuItem("Camera controls", NULL, &win_camera_show);

			ImGui::Separator();

			ImGui::MenuItem("Set skybox", NULL, &win_load_skybox_show);

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::BeginMenu("Scene presets")) 
			{
				if (ImGui::MenuItem("Box")) 
				{
					_Scene->Objects.clear();
					_Scene->Lights->Point.clear();

					load_object("E:/cube.obj");
				}

				if (ImGui::MenuItem("Shadows"))
				{
					_Scene->Objects.clear();
					_Scene->Lights->Point.clear();

					load_object("E:/cube.obj");
					load_object("E:/cube.obj");

					_Scene->Lights->Point.push_back(new PointLight(glm::vec3(0.8f), glm::vec3(0.0f), 1.0, 0.0, 0.007, 0.0002, true));
					_Scene->Objects[0]->Matrix.SetPosition(glm::vec3(0, 0, 5));
					_Scene->Objects[1]->Matrix.SetPosition(glm::vec3(0, 2, 8));
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

}