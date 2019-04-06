#include <Haze.h>

#include "imgui/imgui.h"

class AssimpLayer : public Haze::Layer
{
	public:
		AssimpLayer() : Layer("Assimp Layer") {}

		void OnUpdate() override {}
		void OnEvent(Haze::Event& event) override {}

		void OnImGuiRender() override 
		{
			static std::array<char, 120> buf;
			static Haze::Model* model = nullptr;

			ImGui::Begin("AssimpLoader");
			ImGui::InputText("Filepath", buf.data(), 100);

			if (ImGui::Button("Load")) {
				model = Haze::ModelLoader::Load(std::string(buf.data()));
			}

			if (model) {
				ImGui::Text("Model data");
				ImGui::Text("%d meshes", model->Meshes.size());
				ImGui::BeginChild("Scrolling");

				int i = 1;
				for (auto m : *model) {
					ImGui::Text("[%02d]: %d vertices, %d triangles", i++, m->Vertices.size(), m->Triangles.size());
				}

				ImGui::EndChild();
			} else {
				ImGui::Text("No model loaded!");
			}

			ImGui::End();
		}
};

class ProgramLayer : public Haze::Layer {
	public:
		ProgramLayer() : Layer("Program Layer") {}
		
		void OnUpdate() override {}
		void OnEvent(Haze::Event& event) override {}

		void OnImGuiRender() override {
			static std::array<char, 120> buf;
			static Haze::Program* program = nullptr;

			ImGui::Begin("Program");
			ImGui::InputText("Filepath", buf.data(), 100);

			static bool ver, fra, geo;
			ImGui::Checkbox("Vertex", &ver);
			ImGui::Checkbox("Fragment", &fra);
			ImGui::Checkbox("Geometry", &geo);

			if (ImGui::Button(program ? "Unload" : "Load")) 
			{
				if (program) 
				{
					delete program;
					program = nullptr;
				}
				else 
				{
					program = new Haze::Program(std::string(buf.data()), (ver ? BIT(0) : 0) | (fra ? BIT(1) : 0) | (geo ? BIT(2) : 0));
				}
			}

			static float value = 0.0f;
			static std::array<char, 120> name;
			static int location = -1;
			if (program) {
				ImGui::Text("Program loaded!");

				ImGui::InputText("Uniform", name.data(), 100);

				if (ImGui::Button("Get Location")) {
					program->Bind();
					program->GetUniformLocation(std::string(name.data()));
				}

			} else {
				ImGui::Text("No program loaded!");
			}

			ImGui::End();
		}
};

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new AssimpLayer());
			PushLayer(new ProgramLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}