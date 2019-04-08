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
	CUSTOM_ADAPTER(DefaultAdapter) {
		SCENE {
			_Program->SetUniform("uColor", glm::vec4(1.0f));
		}

		OBJECT {
			_Program->SetUniform("uModelViewProjectionMatrix", camera->GetProjectionMatrix() * camera->GetViewMatrix() * (glm::mat4) object->Matrix);
		}

		MESH {
			_Program->SetUniform("uTexture[0]", mesh->Textures[0] != nullptr);
			_Program->SetUniform("uTexture[1]", mesh->Textures[1] != nullptr);
			_Program->SetUniform("uTexture[2]", mesh->Textures[2] != nullptr);

			_Program->SetUniform("uTexture0", TextureAllocator::Bind(mesh->Textures[0]));
			_Program->SetUniform("uTexture1", TextureAllocator::Bind(mesh->Textures[1]));
			_Program->SetUniform("uTexture2", TextureAllocator::Bind(mesh->Textures[2]));
		}
	};

	void RendererLayer::OnUpdate()
	{
		DefaultAdapter adapter(_Program);
		adapter.Bind();

		adapter.Set(_Scene, _Camera);

		for (auto o : _Scene->Objects) 
		{
			adapter.Set(o, _Camera);

			for (auto m : o->Model->Meshes)
			{
				adapter.Set(m, _Camera);

				m->VAO->Bind();
				m->IBO->Bind();
				glDrawElements(GL_TRIANGLES, m->Triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
			}

			TextureAllocator::Flush();
		}
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

		if (win_load_object_show) win_load_object();
		if (win_list_object_show) win_list_object();
		if (win_camera_show) win_camera();

		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu(std::to_string(frameTime).c_str())) {
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Scene")) 
			{
				ImGui::MenuItem("New object", NULL, &win_load_object_show);
				ImGui::Separator();
				ImGui::MenuItem("Objects", NULL, &win_list_object_show);
				ImGui::EndMenu();
			}

			ImGui::MenuItem("Camera controls", NULL, &win_camera_show);

			ImGui::EndMainMenuBar();
		}
	}

}