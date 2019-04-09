#include "hzpch.h"
#include "Renderer.h"

#include "Program/ProgramAdapter.h"

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
		Draw();
	}

	void RendererLayer::Draw(Mesh* mesh)
	{
		mesh->VAO->Bind();	
		mesh->IBO->Bind();
		glDrawElements(GL_TRIANGLES, mesh->Triangles.size() * 3, GL_UNSIGNED_INT, nullptr);

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

		DrawQuad();

		_Buffer.Copy();
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

		static bool win_load_object_show = false;
		static auto win_load_object = [this]() {
			ImGui::Begin("Add new object", &win_load_object_show);

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Loader flags");
			static bool win_load_object_flags[] = { true, true, true, true };
			ImGui::Checkbox("Triangulate", &win_load_object_flags[0]);
			ImGui::Checkbox("Generate tangents", &win_load_object_flags[1]);
			ImGui::Checkbox("Smooth normals", &win_load_object_flags[2]);
			ImGui::Checkbox("Flip UVs", &win_load_object_flags[3]);
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "File");
			static std::array<char, 200> buffer;
			if (ImGui::InputText("", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
				load_object(std::string(buffer.data()));
				win_load_object_show = false;
			}

			ImGui::End();
		};

		static bool win_list_object_show = false;
		static auto win_list_object = [this]() {
			ImGui::Begin("Objects", &win_list_object_show);

			for (unsigned int i = 0; i < _Scene->Objects.size(); i++) 
			{
				auto object = _Scene->Objects[i];
				auto model = object->Model;
				auto& matrix = object->Matrix;

				if (ImGui::CollapsingHeader(UUID1("Object", i)))
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

						ImGui::TreePop();
					}
				}
			}

			ImGui::End();
		};

		static bool win_camera_show = false;
		static auto win_camera = [this]() {
			ImGui::Begin("Camera", &win_camera_show);

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

			ImGui::End();
		};

		static bool win_list_light_show = false;
		static auto win_list_light = [this]() {
			ImGui::Begin("Lights", &win_list_light_show, ImGuiWindowFlags_MenuBar);

			if (ImGui::BeginMenuBar()) 
			{
				if (_Scene->Lights->Ambient == nullptr && ImGui::MenuItem("Ambient"))  _Scene->Lights->Ambient = new AmbientLight(glm::vec3(1.0f), 0.0f);
				if (_Scene->Lights->Vector == nullptr && ImGui::MenuItem("Vector"))  _Scene->Lights->Vector = new VectorLight(glm::vec3(1.0f), glm::vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f);
				if (ImGui::MenuItem("Point")) _Scene->Lights->Point.push_back(new PointLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.007f, 0.0002f, false));

				ImGui::EndMenuBar();
			}

			if (_Scene->Lights->Ambient) 
			{
				if (ImGui::CollapsingHeader("Ambient light")) 
				{
					ImGui::ColorEdit3("Color##ambient", &_Scene->Lights->Ambient->GetData()[0].x);
					ImGui::Spacing();
					ImGui::SliderFloat("Intensity##ambient", &_Scene->Lights->Ambient->GetData()[2].x, 0.0f, 1.0f);
				}
			}

			ImGui::Spacing();

			if (_Scene->Lights->Vector) 
			{
				if (ImGui::CollapsingHeader("Vector light")) 
				{
					ImGui::ColorEdit3("Color##vector", &_Scene->Lights->Vector->GetData()[0].x);
					ImGui::Spacing();
					ImGui::InputFloat3("Direction##vector", &_Scene->Lights->Vector->GetData()[1].x);
					ImGui::Spacing();
					ImGui::SliderFloat("Intensity##vector", &_Scene->Lights->Vector->GetData()[2].y, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular##vector", &_Scene->Lights->Vector->GetData()[2].z, 0.0f, 1.0f);
				}
			}

			ImGui::Spacing();

			for (unsigned int i = 0; i < _Scene->Lights->Point.size(); i++) 
			{
				if (ImGui::CollapsingHeader(UUID1("Point light", i))) 
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
				}

				ImGui::Spacing();
			}

			ImGui::End();
		};

		if (win_load_object_show) win_load_object();
		if (win_list_object_show) win_list_object();
		if (win_list_light_show) win_list_light();
		if (win_camera_show) win_camera();

		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu(std::to_string(frameTime).c_str())) {
				ImGui::EndMenu();
			}

			ImGui::Separator();

			ImGui::MenuItem("Objects", NULL, &win_list_object_show);
			ImGui::MenuItem("+##0", NULL, &win_load_object_show);

			ImGui::Separator();

			ImGui::MenuItem("Lights", NULL, &win_list_light_show);

			ImGui::Separator();

			ImGui::MenuItem("Camera controls", NULL, &win_camera_show);

			ImGui::EndMainMenuBar();
		}
	}

}