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

namespace Haze
{

	double GUI::Sample() {
		static double last = glfwGetTime();
		static double time = 0;
		static double delt = 0;

		time = glfwGetTime();
		delt = time - last;
		last = time;

		return delt;
	}

	void GUI::FixPath(char* data) {
		for (int i = 0; i < strlen(data); i++) {
			if (data[i] == '\\') {
				data[i] = '/';
			}
		}
	}

	void GUI::BigSeparator() {
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	void GUI::BigSpace() {
		ImGui::Spacing();
		ImGui::Spacing();
	}

	void GUI::ScriptWindow(bool& show, std::array<char, 1000>& content, bool& execute) {
		ImGui::Begin("Script", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Run")) {
				execute = true;
			}
			ImGui::EndMenuBar();
		}
		{
			ImGui::InputTextMultiline("##input", content.data(), content.size(), ImVec2(600, 400));
		}
		ImGui::End();
	}

	void GUI::RepositoryWindow(bool& show) {
		ImGui::Begin("Repository", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Clear")) {
				Repository::_Instance._Pointers.clear();
			}
			ImGui::EndMenuBar();
		}
		{
			for (auto pair : enumerate(Repository::_Instance._Pointers)) {
				ImGui::Columns(3, 0, false);
				ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 160);
				ImGui::SetColumnWidth(1, 80);
				ImGui::SetColumnWidth(2, 80);

				ImGui::Text(pair.value.first.data());

				ImGui::NextColumn();
				ImGui::Text("%X", pair.value.second);

				ImGui::NextColumn();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() - 75);

				ImGui::PushID((int) pair.index);
				if (ImGui::Button("Remove")) {
					Repository::_Instance._Pointers.erase(pair.value.first);
					ImGui::PopID();
					break;
				}

				ImGui::Columns(1);

				ImGui::PopID();
			}
		}
		ImGui::End();
	}

	void GUI::CameraWindow(bool& show, Camera* camera) {
		ImGui::Begin("Camera", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(400, 185));
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Reset")) {
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

			if (refresh) {
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

	void GUI::InsertObjectWindow(bool& show, Scene* scene) {
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
			ImGui::PushItemWidth(300);
			ImGui::InputText(".obj", buffer.data(), buffer.size());
			ImGui::SameLine();

			if (ImGui::Button("Open")) {
				FixPath(buffer.data());
				Model* model = ModelLoader::Load(std::string(buffer.data()) + ".obj");
				if (model) {
					Object* object = new Object();
					object->Model = model;
					scene->Objects.push_back(object);
				}

				show = false;
			}
		}

		ImGui::End();
	}

	void GUI::SetSkyboxWindow(bool& show, Scene* scene) {
		ImGui::Begin("Set skybox", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Set skybox", ImVec2(400, 85));
		{
			static std::array<char, 200> buffer;

			ImGui::Text("Filepath");
			ImGui::PushItemWidth(330);
			ImGui::InputText("##input", buffer.data(), buffer.size());
			ImGui::SameLine();
			if (ImGui::Button("Open")) {
				FixPath(buffer.data());
				Texture* skybox = TextureLoader::LoadCube(std::string(buffer.data()));
				if (skybox) {
					if (scene->Skybox) {
						delete scene->Skybox;
					}

					scene->Skybox = static_cast<TextureCube*>(skybox);
				}

				show = false;
			}
		}
		ImGui::End();
	}

	void GUI::ObjectManagerWindow(bool& show, bool& show_insertobject, Scene* scene, Camera* camera) {
		static std::string preview = "";

		if (atoi(preview.data()) >= scene->Objects.size()) preview = "";

		ImGui::Begin("Object Manager", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			if (ImGui::BeginMenuBar()) {
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
			if (ImGui::BeginCombo("##combo", preview.data())) {
				for (auto obj : enumerate(scene->Objects)) {
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

			if (preview != "") {
				int index = atoi(preview.data());
				auto obj = scene->Objects[index];

				if (ImGui::Button("Remove")) {
					scene->Objects.erase(scene->Objects.begin() + index);
					delete obj;
					preview = "";
				}

				ImGui::SameLine(ImGui::GetWindowWidth() - 100);
				if (ImGui::Button("Move to aim")) {
					scene->Objects[index]->Matrix.SetPosition(camera->GetWorldPointer().first);
				}

				if (ImGui::CollapsingHeader("Details")) {
					bool expanded = ImGui::TreeNode("Meshes");
					ImGui::SameLine(ImGui::GetWindowWidth() - 60);
					ImGui::Text("%d", obj->Model->Meshes.size());

					if (expanded) {
						for (auto m : enumerate(obj->Model->Meshes)) {
							ImGui::PushID((int) m.index);
							if (ImGui::TreeNode(std::to_string(m.index).data())) {
								ImGui::Columns(2, 0, false);
								ImGui::Text("Vertices:");
								ImGui::Text("Triangles:");
								ImGui::NextColumn();
								ImGui::Text("%d", m.value->Vertices.size());
								ImGui::Text("%d", m.value->Triangles.size());
								ImGui::Columns(1);

								if (ImGui::TreeNode("Textures")) {
									for (int i = 0; i < 3; i++) {
										Texture* texture = m.value->Textures[i];
										static std::string textureNames[] = { "Diffuse", "Normal", "Specular" };

										if (texture) {
											ImGui::PushID(i);
											if (ImGui::TreeNode(textureNames[i].data())) {
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

				if (ImGui::CollapsingHeader("Transformations", ImGuiTreeNodeFlags_DefaultOpen)) {
					bool refresh = false;
					refresh |= ImGui::InputFloat3("Position", &obj->Matrix._Position.x);
					refresh |= ImGui::SliderFloat3("Scale", &obj->Matrix._Scale.x, 0.1f, 10.0f, "%.1f");
					refresh |= ImGui::SliderFloat3("Rotation", &obj->Matrix._Rotation.x, -3.14f, 3.14f, "%.2f");

					if (refresh) {
						obj->Matrix.UpdateMatrices(true);
					}
				}
			}
		}
		ImGui::End();
	}

	void GUI::LightManagerWindow(bool& show, Scene* scene, Camera* camera) {
		ImGui::Begin("Light Manager", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			if (ImGui::BeginMenuBar()) {
				if (ImGui::MenuItem("Reset")) {
					delete scene->Lights->Ambient;
					delete scene->Lights->Vector;
					for (auto& l : scene->Lights->Point) delete l;

					scene->Lights->Ambient = new AmbientLight(glm::vec3(1.0f), 0.1f);
					scene->Lights->Vector = new VectorLight(glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.1f);
					scene->Lights->Point.clear();
				}

				if (ImGui::MenuItem("Add light")) {
					scene->Lights->Point.push_back(new PointLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.007f, 0.0002f, false));
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

			BigSpace();

			for (auto light : enumerate(scene->Lights->Point)) {
				ImGui::PushID((int) light.index);

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

				if (expanded) {
					ImGui::Spacing();
					ImGui::ColorEdit3("Color", &data[0].x);
					ImGui::Spacing();
					ImGui::InputFloat3("Position", &data[1].x);
					ImGui::Spacing();
					ImGui::SliderFloat("Intensity", &data[2].y, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &data[2].z, 0.0f, 1.0f);
					ImGui::Spacing();
					ImGui::InputFloat("Linear", &data[3].y, 0, 0, "%.4f");
					ImGui::InputFloat("Quadratic", &data[3].z, 0, 0, "%.4f");
					ImGui::Spacing();
					ImGui::Checkbox("Casts shadow", &light.value->IsCastingShadow());
					ImGui::Spacing();
					ImGui::Columns(2, 0, false);
					if (ImGui::Button("Set using camera")) data[1] = camera->GetWorldPosition();
					ImGui::Columns(1);
				}

				ImGui::PopID();

				BigSpace();
			}
		}
		ImGui::End();
	}

}