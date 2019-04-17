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
		static double samples[120];
		static int samples_cnt = 0;

		double time = glfwGetTime();
		samples[samples_cnt++ % 120] = time - last;
		last = time;

		double average = 0.0;
		for (int i = 0; i < 120; i++) 
		{
			average += samples[i];
		}

		return average / 120.0;
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

	bool GUI::InFocus() 
	{
		return !ImGui::IsAnyWindowFocused();
	}

	void GUI::ScriptWindow(bool& show, Scene* scene, Camera* camera) 
	{
		static std::array<char, 5000> script;
		static std::array<char, 100> script_dump_location;

		ImGui::Begin("Script", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImGui::BeginMenuBar();
			if (ImGui::MenuItem("Run")) {
				Interpreter::ExecuteScript(script.data(), scene, camera);
			}

			ImGui::Separator();

			ImGui::Text("File: ");

			if (ImGui::MenuItem("Dump")) {
				if (std::string(script_dump_location.data()).size() > 0) {
					Interpreter::Dump(std::string(script_dump_location.data()), scene, camera);
				}
			}
			if (ImGui::MenuItem("Run")) {
				std::fstream ifs(std::string(script_dump_location.data()) , std::ios::in);
				if (ifs.is_open()) {
					std::stringstream ss;
					std::string s;
					while (getline(ifs, s)) ss << s;

					Interpreter::ExecuteScript(ss.str().data(), scene, camera);
				}
			}
		
			ImGui::InputText("##outscript", script_dump_location.data(), script_dump_location.size());
			ImGui::EndMenuBar();
		}
		{
			ImGui::InputTextMultiline("##input", script.data(), script.size(), ImVec2(600, 400));
		}
		ImGui::End();
	}

	void GUI::RepositoryWindow(bool& show) {
		ImGui::Begin("Repository", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			for (auto pair : enumerate(Repository::_Instance._PtrNameMap)) {
				ImGui::Columns(3, 0, false);
				ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 180);
				ImGui::SetColumnWidth(1, 100);
				ImGui::SetColumnWidth(2, 80);

				ImGui::Text(pair.value.first.data());

				ImGui::NextColumn();
				ImGui::Text("%X", pair.value.second);

				ImGui::NextColumn();
				ImGui::Text("%d", Repository::_Instance._PtrCallMap[pair.value.second]);

				ImGui::Columns(1);
			}
		}
		ImGui::End();
	}

	void GUI::Menu(Scene* scene, Camera* camera, int& drawmode, int& normals)
	{
		static bool win_camera = false;
		static bool win_repository = false;
		static bool win_setskybox = false;
		static bool win_insertobject = false;
		static bool win_objectmanager = false;
		static bool win_lightmanager = false;
		static bool win_script = false;
		static bool win_normals[3] = { false, false, false };

		if (win_camera) GUI::CameraWindow(win_camera, camera);
		if (win_repository) GUI::RepositoryWindow(win_repository);
		if (win_script) GUI::ScriptWindow(win_script, scene, camera);
		if (win_setskybox) GUI::SetSkyboxWindow(win_setskybox, scene);
		if (win_insertobject) GUI::InsertObjectWindow(win_insertobject, scene);
		if (win_objectmanager) GUI::ObjectManagerWindow(win_objectmanager, win_insertobject, scene, camera);
		if (win_lightmanager) GUI::LightManagerWindow(win_lightmanager, scene, camera);

		if (ImGui::BeginMainMenuBar()) {
			double frametime = GUI::Sample();
			if (frametime < 0.018) ImGui::TextColored(ImVec4(RGBTOF(23, 91, 37)), "%.4F", frametime);
			else if (frametime < 0.021) ImGui::TextColored(ImVec4(RGBTOF(211, 214, 23)), "%.4F", frametime);
			else ImGui::TextColored(ImVec4(RGBTOF(214, 64, 23)), "%.4F", frametime);
			GUI::BigSeparator();

			if (ImGui::BeginMenu("HAZE")) {
				if (ImGui::BeginMenu("Drawing mode")) {
					ImGui::RadioButton("Default", &drawmode, 0);
					GUI::BigSpace();
					ImGui::RadioButton("Position", &drawmode, 1);
					ImGui::RadioButton("Normals", &drawmode, 2);
					ImGui::RadioButton("Shadows", &drawmode, 5);
					GUI::BigSeparator();
					ImGui::Text("Textures");
					GUI::BigSpace();
					ImGui::RadioButton("Diffuse", &drawmode, 3);
					ImGui::RadioButton("Specular", &drawmode, 4);
					ImGui::EndMenu();
				}
				GUI::BigSeparator();
				if (ImGui::BeginMenu("Overlays")) {
					ImGui::Checkbox("(V) Normals", win_normals);
					ImGui::Checkbox("(F) Normals", win_normals + 1);
					ImGui::Checkbox("No CULLFACE", win_normals + 2);
					ImGui::EndMenu();
				}
				GUI::BigSeparator();
				ImGui::MenuItem("Repository", 0, &win_repository);
				ImGui::EndMenu();
			}
			GUI::BigSeparator();
			if (ImGui::BeginMenu("Scene")) {
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

			normals = normals & ~BIT(0) | (win_normals[0] << 0);
			normals = normals & ~BIT(1) | (win_normals[1] << 1);
			normals = normals & ~BIT(2) | (win_normals[2] << 2);
		}
	}

	void GUI::CameraWindow(bool& show, Camera* camera) {
		ImGui::Begin("Camera", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(400, 210));
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
			bool refresh2 = ImGui::SliderFloat("Fov", &camera->_Fov, 30.0f, 120.0f, "%.0f");

			if (refresh2) {
				camera->SetFov(camera->_Fov);
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

					ImGui::End();
					return;
				}

				ImGui::SameLine(ImGui::GetWindowWidth() - 100);
				ImGui::Text(obj->Model->Name.data());

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
					scene->Ambient.Reset();
					scene->Vector.Reset();
					scene->Point.clear();
				}

				if (ImGui::MenuItem("Add light")) {
					scene->Point.emplace_back(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.007f, 0.0002f, false);
				}

				ImGui::EndMenuBar();
			}
		}
		{
			if (ImGui::CollapsingHeader("Ambient light")) {
				ImGui::Spacing();
				ImGui::ColorEdit3("Color##ambient", &scene->Ambient.GetData()[0].x);
				ImGui::Spacing();
				ImGui::SliderFloat("Intensity##ambient", &scene->Ambient.GetData()[2].x, 0.0f, 1.0f);
			}

			GUI::BigSpace();

			if (ImGui::CollapsingHeader("Vector light")) {
				ImGui::Spacing();
				ImGui::ColorEdit3("Color##vector", &scene->Vector.GetData()[0].x);
				ImGui::Spacing();
				ImGui::InputFloat3("Direction##vector", &scene->Vector.GetData()[1].x);
				ImGui::Spacing();
				ImGui::SliderFloat("Intensity##vector", &scene->Vector.GetData()[2].y, 0.0f, 1.0f);
				ImGui::SliderFloat("Specular##vector", &scene->Vector.GetData()[2].z, 0.0f, 1.0f);
				ImGui::Spacing();
				if (ImGui::Button("Set using camera")) scene->Vector.GetData()[1] = camera->GetDirection();
			}

			BigSpace();

			for (auto& light : enumerate(scene->Point)) {
				ImGui::PushID((int) light.index);

				bool expanded = ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_AllowOverlapMode);
				ImGui::SameLine(ImGui::GetWindowWidth() - 70);
				if (ImGui::Button("Remove")) {
					scene->Point.erase(scene->Point.begin() + light.index);
					ImGui::PopID();
					break;
				}

				auto& data = light.value.GetData();

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
					ImGui::Checkbox("Casts shadow", &light.value.IsCastingShadow());
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