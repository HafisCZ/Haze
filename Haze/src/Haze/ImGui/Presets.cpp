#include "hzpch.h"
#include "Presets.h"

#include "Haze/enumerate.h"

#include "Haze/Repository.h"
#include "Haze/Scene/Camera.h"
#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Light.h"

#include "imgui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GUI 
{

	double Sample() 
	{
		static double last = glfwGetTime();
		static double time = 0;
		static double delt = 0;

		time = glfwGetTime();
		delt = time - last;
		last = time;

		return delt;
	}

	void FixPath(char* data) {
		for (int i = 0; i < strlen(data); i++) 
		{
			if (data[i] == '\\') 
			{
				data[i] = '/';
			}
		}
	}

	void Test()
	{
		ImGui::ShowTestWindow();
	}

	void BigSeparator()
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	void BigSpace() 
	{
		ImGui::Spacing();
		ImGui::Spacing();
	}

	void ScriptWindow(bool& show, std::array<char, 1000>& content, bool& execute) 
	{
		ImGui::Begin("Script", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Run")) 
			{
				execute = true;
			}
			ImGui::EndMenuBar();
		}
		{
			ImGui::InputTextMultiline("##input", content.data(), content.size(), ImVec2(600, 400));
		}
		ImGui::End();
	}

	void RepositoryWindow(bool& show)
	{
		ImGui::Begin("Repository", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Clear")) 
			{
				Haze::Repository::_Instance._Pointers.clear();
			}
			ImGui::EndMenuBar();
		}
		{
			for (auto pair : enumerate(Haze::Repository::_Instance._Pointers))
			{
				ImGui::Columns(3, 0, false);
				ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 160);
				ImGui::SetColumnWidth(1, 80);
				ImGui::SetColumnWidth(2, 80);

				ImGui::Text(pair.value.first.data());

				ImGui::NextColumn();
				ImGui::Text("%X", pair.value.second);

				ImGui::NextColumn();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() - 75);

				ImGui::PushID(pair.index);
				if (ImGui::Button("Remove")) 
				{
					Haze::Repository::_Instance._Pointers.erase(pair.value.first);
					ImGui::PopID();
					break;
				}

				ImGui::Columns(1);

				ImGui::PopID();
			}
		}
		ImGui::End();
	}

	void CameraWindow(bool& show, Haze::Camera* camera) 
	{
		ImGui::Begin("Camera", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(400, 185));
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Reset")) 
			{
				camera->SetSpeed(0.4f, 0.2f);
				camera->Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			}
			ImGui::EndMenuBar();
		}
		{
			bool refresh = false;
			refresh |= ImGui::InputFloat3("Position", &camera->_WorldPosition[0]);

			ImGui::Spacing();

			refresh |= ImGui::SliderFloat("Yaw", &camera->_Yaw, -360.0f, 360.0f, "%.0f");
			refresh |= ImGui::SliderFloat("Pitch", &camera->_Pitch, -89.0f, 89.0f, "%.0f");

			if (refresh) 
			{
				camera->UpdateMatrices();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::InputFloat("Speed", &camera->_MoveSpeed);
			ImGui::InputFloat("Sensitivity", &camera->_LookSpeed);
		}
		ImGui::End();
	}

	void InsertObjectWindow(bool& show, Haze::Scene* scene) 
	{
		ImGui::Begin("Add new object", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Add new object", ImVec2(400, 150));
		{
			static std::array<char, 200> buffer;
			static bool win_load_object_flags[] = { true, true, true, true };

			ImGui::Text("Loader flags");

			ImGui::Columns(2, 0, false);

			ImGui::Checkbox("Triangulate", &win_load_object_flags[0]);
			ImGui::Checkbox("Generate tangents", &win_load_object_flags[1]);

			ImGui::NextColumn();
			ImGui::Checkbox("Smooth normals", &win_load_object_flags[2]);
			ImGui::Checkbox("Flip UVs", &win_load_object_flags[3]);

			ImGui::Columns(1);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Filepath");
			ImGui::PushItemWidth(350);
			if (ImGui::InputText(".obj", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				FixPath(buffer.data());
				Haze::Model* model = Haze::ModelLoader::Load(std::string(buffer.data()) + ".obj");
				if (model) {
					Haze::Object* object = new Haze::Object();
					object->Model = model;
					scene->Objects.push_back(object);
				}

				show = false;
			}
		}
		ImGui::End();		
	}

	void SetSkyboxWindow(bool& show, Haze::Scene* scene) 
	{
		ImGui::Begin("Set skybox", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Set skybox", ImVec2(400, 85));
		{
			static std::array<char, 200> buffer;

			ImGui::Text("Filepath");
			ImGui::PushItemWidth(380);
			if (ImGui::InputText("##input", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) 
			{
				FixPath(buffer.data());
				Haze::Texture* skybox = Haze::TextureLoader::LoadCube(std::string(buffer.data()));
				if (skybox) 
				{
					if (scene->Skybox) 
					{
						delete scene->Skybox;
					}

					scene->Skybox = static_cast<Haze::TextureCube*>(skybox);
				}

				show = false;
			}
		}
		ImGui::End();
	}

	void ObjectManagerWindow(bool& show, bool& show_insertobject, Haze::Scene* scene)
	{
		static std::string preview = "";

		ImGui::Begin("Object Manager", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			if (ImGui::BeginMenuBar()) 
			{
				if (ImGui::MenuItem("Reset")) {
					for (auto& o : scene->Objects) delete o;
					scene->Objects.clear();
					preview = "";
				}

				if (ImGui::MenuItem("Add object")) show_insertobject = true;

				ImGui::EndMenuBar();
			}
		}
		{
			ImGui::PushItemWidth(ImGui::GetWindowWidth() - 30);
			if (ImGui::BeginCombo("##combo", preview.data())) 
			{
				for (auto obj : enumerate(scene->Objects)) 
				{
					std::string str(std::to_string(obj.index));

					bool selected = obj.index == atoi(preview.data());
					if (ImGui::Selectable(str.data(), selected)) {
						preview = str;
					}

					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			GUI::BigSeparator();

			if (preview != "") 
			{
				int index = atoi(preview.data());
				auto obj = scene->Objects[index];

				if (ImGui::Button("Remove")) 
				{
					scene->Objects.erase(scene->Objects.begin() + index);
					delete obj;
					preview = "";
				}

				if (ImGui::CollapsingHeader("Details")) 
				{
					bool expanded = ImGui::TreeNode("Meshes");
					ImGui::SameLine(ImGui::GetWindowWidth() - 60);
					ImGui::Text("%d", obj->Model->Meshes.size());

					if (expanded) 
					{
						for (auto m : enumerate(obj->Model->Meshes)) 
						{
							ImGui::PushID(m.index);
							if (ImGui::TreeNode(std::to_string(m.index).data())) 
							{
								ImGui::Columns(2, 0, false);
								ImGui::Text("Vertices:");
								ImGui::Text("Triangles:");
								ImGui::NextColumn();
								ImGui::Text("%d", m.value->Vertices.size());
								ImGui::Text("%d", m.value->Triangles.size());
								ImGui::Columns(1);
							
								if (ImGui::TreeNode("Textures")) 
								{
									for (int i = 0; i < 3; i++) 
									{
										Haze::Texture* texture = m.value->Textures[i];
										static std::string textureNames[] = { "Diffuse", "Normal", "Specular" };
									
										if (texture)
										{
											ImGui::PushID(i);
											if (ImGui::TreeNode(textureNames[i].data()))
											{
												ImGui::Columns(2, 0, false);
												ImGui::Text("Width");
												ImGui::Text("Height");
												ImGui::Text("Channels");
												ImGui::Text("Binds");
												ImGui::NextColumn();
												ImGui::Text("%d", texture->_Width);
												ImGui::Text("%d", texture->_Height);
												ImGui::Text("%d", texture->_PPM);
												ImGui::Text("%d", texture->_TotalBinds);
												ImGui::Columns(1);
												ImGui::TreePop();
											}
											ImGui::PopID();
										}
									}

									ImGui::TreePop();
								}

								ImGui::TreePop();
							}
							ImGui::PopID();
						}

						ImGui::TreePop();
					}
				}

				if (ImGui::CollapsingHeader("Transformations", ImGuiTreeNodeFlags_DefaultOpen)) 
				{
					bool refresh = false;
					refresh |= ImGui::InputFloat3("Position", &obj->Matrix._Position.x);
					refresh |= ImGui::SliderFloat3("Scale", &obj->Matrix._Scale.x, 0.1f, 10.0f, "%.1f");
					refresh |= ImGui::SliderFloat3("Rotation", &obj->Matrix._Rotation.x, -3.14f, 3.14f, "%.2f");

					if (refresh) 
					{
						obj->Matrix.UpdateMatrices(true);
					}
				}
			}
		}
		ImGui::End();
	}

	void LightManagerWindow(bool& show, Haze::Scene* scene, Haze::Camera* camera)
	{
		ImGui::Begin("Light Manager", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			if (ImGui::BeginMenuBar()) 
			{
				if (ImGui::MenuItem("Reset")) 
				{
					delete scene->Lights->Ambient;
					delete scene->Lights->Vector;
					for (auto& l : scene->Lights->Point) delete l;

					scene->Lights->Ambient = new Haze::AmbientLight(glm::vec3(1.0f), 0.1f);
					scene->Lights->Vector = new Haze::VectorLight(glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.1f);
					scene->Lights->Point.clear();
				}

				if (ImGui::MenuItem("Add light")) 					
				{
					scene->Lights->Point.push_back(new Haze::PointLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.007f, 0.0002f, false));
				}

				ImGui::EndMenuBar();
			}
		}
		{
			if (ImGui::CollapsingHeader("Ambient light")) {
				ImGui::Spacing();
				ImGui::ColorEdit3("Color##ambient", &scene->Lights->Ambient->GetData()[0].x);
				ImGui::Spacing();
				ImGui::SliderFloat("Intensity##ambient", &scene->Lights->Ambient->GetData()[2].x, 0.0f, 1.0f);
			}

			GUI::BigSpace();

			if (ImGui::CollapsingHeader("Vector light")) {
				ImGui::Spacing();
				ImGui::ColorEdit3("Color##vector", &scene->Lights->Vector->GetData()[0].x);
				ImGui::Spacing();
				ImGui::InputFloat3("Direction##vector", &scene->Lights->Vector->GetData()[1].x);
				ImGui::Spacing();
				ImGui::SliderFloat("Intensity##vector", &scene->Lights->Vector->GetData()[2].y, 0.0f, 1.0f);
				ImGui::SliderFloat("Specular##vector", &scene->Lights->Vector->GetData()[2].z, 0.0f, 1.0f);
				ImGui::Spacing();
				if (ImGui::Button("Set using camera")) scene->Lights->Vector->GetData()[1] = camera->GetDirection();
			}

			GUI::BigSpace();

			for (auto light : enumerate(scene->Lights->Point))
			{
				ImGui::PushID(light.index);

				bool expanded = ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_AllowOverlapMode);
				ImGui::SameLine(ImGui::GetWindowWidth() - 70);
				if (ImGui::Button("Remove")) {
					auto l = light.value;
					scene->Lights->Point.erase(scene->Lights->Point.begin() + light.index);
					delete l;

					ImGui::PopID();
					break;
				}

				auto& data = light.value->GetData();

				if (expanded) 
				{
					ImGui::Spacing();
					ImGui::ColorEdit3("Color", &data[0].x);
					ImGui::Spacing();
					ImGui::InputFloat3("Position", &data[1].x);
					ImGui::Spacing();
					ImGui::SliderFloat("Intensity", &data[2].y, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &data[2].z, 0.0f, 1.0f);
					ImGui::Spacing();
					ImGui::Checkbox("Casts shadow", &light.value->IsCastingShadow());
					ImGui::Spacing();
					ImGui::Columns(2, 0, false);
					if (ImGui::Button("Set using camera")) data[1] = camera->GetWorldPosition();
					ImGui::Columns(1);
				}

				ImGui::PopID();

				GUI::BigSpace();
			}
		}
		ImGui::End();
	}

}