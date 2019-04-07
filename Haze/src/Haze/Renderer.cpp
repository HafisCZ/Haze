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
		
			_Program->SetUniform("uModelViewProjectionMatrix", _Camera->GetProjectionMatrix() * _Camera->GetViewMatrix());

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

			if (ImGui::InputFloat3("Position", &_Camera->_WorldPosition[0])) _Camera->UpdateMatrices();
			ImGui::Separator();

			if (ImGui::SliderFloat("Yaw", &_Camera->_Yaw, -89.0f, 89.0f)) _Camera->UpdateMatrices();
			if (ImGui::SliderFloat("Pitch", &_Camera->_Pitch, -180.0f, 180.0f)) _Camera->UpdateMatrices();
			ImGui::Separator();

			if (ImGui::ArrowButton("L", ImGuiDir_Left)) {
				_Camera->Move(-1.0f, 0.0f, 0.0f);
				_Camera->UpdateMatrices();
			}
			ImGui::SameLine();
			if (ImGui::ArrowButton("F", ImGuiDir_Up)) {
				_Camera->Move(0.0f, 0.0f, 1.0f);
				_Camera->UpdateMatrices();
			}
			ImGui::SameLine();
			if (ImGui::ArrowButton("B", ImGuiDir_Down)) {
				_Camera->Move(0.0f, 0.0f, -1.0f);
				_Camera->UpdateMatrices();
			}
			ImGui::SameLine();
			if (ImGui::ArrowButton("R", ImGuiDir_Right)) {
				_Camera->Move(1.0f, 0.0f, 0.0f);
				_Camera->UpdateMatrices();
			}

			if (ImGui::Button("Up")) {
				_Camera->Move(0.0f, 1.0f, 0.0f);
				_Camera->UpdateMatrices();
			}
			ImGui::SameLine();
			if (ImGui::Button("Down")) {
				_Camera->Move(0.0f, -1.0f, 0.0f);
				_Camera->UpdateMatrices();
			}

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