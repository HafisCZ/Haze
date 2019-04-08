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
		for (auto o : _Scene->Objects) {
			_Program->Bind();
			_Program->SetUniform("uColor", glm::vec4(1.0f));
			_Program->SetUniform("uModelViewProjectionMatrix", _Camera->GetProjectionMatrix() * _Camera->GetViewMatrix() * (glm::mat4) o->Matrix);

			for (auto m : o->Model->Meshes)
			{
				_Program->SetUniform("uTexture[0]", m->Textures[0] != nullptr);
				_Program->SetUniform("uTexture[1]", m->Textures[1] != nullptr);
				_Program->SetUniform("uTexture[2]", m->Textures[2] != nullptr);

				_Program->SetUniform("uTexture0", TextureAllocator::Bind(m->Textures[0]));
				_Program->SetUniform("uTexture1", TextureAllocator::Bind(m->Textures[1]));
				_Program->SetUniform("uTexture2", TextureAllocator::Bind(m->Textures[2]));

				m->VAO->Bind();
				m->IBO->Bind();
				glDrawElements(GL_TRIANGLES, m->Triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
			}

			TextureAllocator::Flush();
		}
	}

	void RendererLayer::OnImGuiRender()
	{
		static std::array<char, 150> buffer;
		static auto try_load_object = [this](const std::string& path) {
			Model* model = ModelLoader::Load(path);
			if (model) {
				Object* object = new Object();
				object->Model = model;
			
				_Scene->Objects.push_back(object);
			}
		};

		ImGui::Begin("Scene");
		ImGui::Text("Path: "); ImGui::SameLine(); ImGui::InputText("##Path", buffer.data(), 140); ImGui::SameLine(); 
		if (ImGui::Button("Load")) try_load_object(std::string(buffer.data()));
		ImGui::Separator();

		for (unsigned int i = 0; i < _Scene->Objects.size(); i++)
		{
			auto object = _Scene->Objects[i];
			auto model = object->Model;
			auto& matrix = object->Matrix;

			if (ImGui::CollapsingHeader(std::string("Object " + std::to_string(i)).c_str()))
			{
				if (ImGui::TreeNode(std::string("Meshes##" + std::to_string(i)).c_str())) 
				{
					for (unsigned int j = 0; j < model->Meshes.size(); j++) {
						if (ImGui::TreeNode(std::string(std::to_string(j) + "##" + std::to_string(i)).c_str())) {
							ImGui::Text("Vertices");	ImGui::SameLine(200);	ImGui::Text("%d", model->Meshes[j]->Vertices.size());
							ImGui::Text("Triangles");	ImGui::SameLine(200);	ImGui::Text("%d", model->Meshes[j]->Triangles.size());

							if (ImGui::TreeNode(std::string("Textures##" + std::to_string(i)).c_str())) 
							{
								static std::string textureNames[] = { "Diffuse", "Normal", "Specular" };
								for (unsigned int k = 0; k < 3; k++) {
									if (model->Meshes[j]->Textures[k]) {
										if (ImGui::TreeNode(std::string(textureNames[k] + "##" + std::to_string(i * 100000 + j * 1000 + k)).c_str())) {
											auto texture = model->Meshes[j]->Textures[k];

											ImGui::Text("Width"); ImGui::SameLine(200); ImGui::Text("%d", texture->_Width);
											ImGui::Text("Height"); ImGui::SameLine(200); ImGui::Text("%d", texture->_Height);
											ImGui::Text("Channels"); ImGui::SameLine(200); ImGui::Text("%d", texture->_PPM);
											ImGui::Spacing();
											ImGui::Text("Binds"); ImGui::SameLine(200); ImGui::Text("%d", texture->_TotalBinds);
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
				if (ImGui::TreeNode(std::string("Transformations##" + std::to_string(i)).c_str())) {
					ImGui::Text("Position:"); ImGui::SameLine(200); if (ImGui::InputFloat3(std::string("##pos" + std::to_string(i)).c_str(), &matrix._Position.x)) matrix.UpdateMatrices(true);
					ImGui::Text("Scale:"); ImGui::SameLine(200); if (ImGui::SliderFloat3(std::string("##scl" + std::to_string(i)).c_str(), &matrix._Scale.x, 0.1f, 10.0f, "%.1f")) matrix.UpdateMatrices(true);
					ImGui::Text("Rotation:"); ImGui::SameLine(200); if (ImGui::SliderFloat3(std::string("##rot" + std::to_string(i)).c_str(), &matrix._Rotation.x, -3.14f, 3.14f, "%.2f")) matrix.UpdateMatrices(true);
					ImGui::TreePop();
				}
			}
		}

		ImGui::End();

		ImGui::Begin("Camera control");

		if (ImGui::InputFloat3("Position", &_Camera->_WorldPosition[0])) _Camera->UpdateMatrices();
		ImGui::Separator();

		if (ImGui::SliderFloat("Yaw", &_Camera->_Yaw, -89.0f, 89.0f, "%.0f")) _Camera->UpdateMatrices();
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
	}

}