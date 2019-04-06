#include <Haze.h>

#include "imgui/imgui.h"

class SandboxLayer : public Haze::Layer
{
	public:
		Haze::Model* model = nullptr;

		std::array<char, 120> buf;

		SandboxLayer() : Layer("Sandbox Layer") 
		{
			buf.fill(0);
		}

		void OnUpdate() override 
		{

		}

		void OnEvent(Haze::Event& event) override 
		{

		}

		void OnImGuiRender() override 
		{
			ImGui::Begin("AssimpLoader");
			ImGui::InputText("##Filepath", buf.data(), 100);

			if (ImGui::Button("Load")) {
				model = Haze::ModelLoader::Load(std::string(buf.data()));
			}
			
			if (model) {
				ImGui::Text("Model data");
				ImGui::Text("%d meshes", model->Meshes.size());
				ImGui::BeginChild("Scrolling");

				int i = 1;
				for (auto m : *model) {
					ImGui::Text("[%02d]: %d vertices, %d indices", i++, m->Vertices, m->Indices);
				}

				ImGui::EndChild();
			} else {
				ImGui::Text("No model loaded!");
			}

			ImGui::End();
		}
};

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new SandboxLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}