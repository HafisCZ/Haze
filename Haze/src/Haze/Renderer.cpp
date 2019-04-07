#include "hzpch.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "STB/stb_image.h"

namespace Haze 
{

	void RendererLayer::OnUpdate()
	{
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(_CameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modl = glm::mat4(1.0f);

		if (_Model)
		{
			_Program->Bind();
			_Program->SetUniform("uColor", _Color);

			if (_Texture) {
				_Program->SetUniform("uTextureEnable", 1);
				_Program->SetUniform("uTexture", TextureAllocator::Bind(_Texture));
			} else {
				_Program->SetUniform("uTextureEnable", 0);
				_Program->SetUniform("uTexture", 0);
			}
		
			_Program->SetUniform("uModelViewProjectionMatrix", proj * view * modl);

			for (auto m : _Model->Meshes)
			{
				m->VAO->Bind();
				m->IBO->Bind();

				glDrawElements(GL_TRIANGLES, m->Triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
			}

			TextureAllocator::Flush();
		}
	}

	void RendererLayer::OnImGuiRender()
	{
		static bool show_model_loader = false;
		static auto func_model_loader = [this](bool& show) {
			ImGui::Begin("Model Loader", &show);

			static std::array<char, 120> buffer;

			ImGui::Text("Filepath:");	ImGui::SameLine();
			ImGui::InputText("##wid", buffer.data(), buffer.size() - 1);	ImGui::SameLine();

			if (ImGui::Button("Open")) {
				_Model = ModelLoader::Load(std::string(buffer.data()));
			}

			ImGui::Separator();

			if (_Model) {
				ImGui::Text("Triangles:");	ImGui::SameLine(200);	ImGui::Text("%d", [this]() -> unsigned int { unsigned int count = 0; for (auto m : _Model->Meshes) count += m->Triangles.size(); return count; }());
				ImGui::Text("Mesh count:");	ImGui::SameLine(200);	ImGui::Text("%d", _Model->Meshes.size());
				ImGui::Separator();

				if (ImGui::CollapsingHeader("Meshes")) {
					for (unsigned int i = 0; i < _Model->Meshes.size(); i++) {
						if (ImGui::TreeNode(std::string("Mesh " + std::to_string(i)).c_str())) {
							ImGui::Text("Vertices");	ImGui::SameLine(200);	ImGui::Text("%d", _Model->Meshes[i]->Vertices.size());
							ImGui::Text("Triangles");	ImGui::SameLine(200);	ImGui::Text("%d", _Model->Meshes[i]->Triangles.size());
							ImGui::TreePop();
						}
					}
				}
			} else {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No model opened!");
			}

			ImGui::End();
		};

		static bool show_camera_control = false;
		static auto func_camera_control = [this](bool& show) {
			ImGui::Begin("Camera control", &show);

			ImGui::InputFloat3("Position: ", &_CameraPosition[0]);

			ImGui::End();
		};

		static bool show_texture_control = false;
		static auto func_texture_control = [this](bool& show) {
			ImGui::Begin("Texture control", &show);

			ImGui::ColorPicker3("Color", &_Color[0]);

			ImGui::Separator();

			static std::array<char, 120> buffer;

			ImGui::Text("Filepath:");	ImGui::SameLine();
			ImGui::InputText("##wid", buffer.data(), buffer.size() - 1);	ImGui::SameLine();

			if (ImGui::Button("Open")) {
				_Texture = TextureLoader::Load(std::string(buffer.data()));
			}

			ImGui::Separator();

			if (_Texture) {
				ImGui::Text("Width:");	ImGui::SameLine(200);	ImGui::Text("%d", _Texture->_Width);
				ImGui::Text("Height:");	ImGui::SameLine(200);	ImGui::Text("%d", _Texture->_Height);
				ImGui::Text("PPM:");	ImGui::SameLine(200);	ImGui::Text("%d", _Texture->_PPM);
				ImGui::Text("Format:");	ImGui::SameLine(200);	ImGui::Text("%d", _Texture->_Format);
			} else {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No texture opened!");
			}

			ImGui::End();
		};

		if (show_model_loader) func_model_loader(show_model_loader);
		if (show_camera_control) func_camera_control(show_camera_control);
		if (show_texture_control) func_texture_control(show_texture_control);

		ImGui::Begin("Render controller");
		ImGui::Checkbox("Model", &show_model_loader);
		ImGui::Checkbox("Camera", &show_camera_control);
		ImGui::Checkbox("Texture", &show_texture_control);
		ImGui::End();
	}

}