#include "hzpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#define IMGUI_IMPL_API
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "Haze/Application.h"
#include "Haze/Program/Program.h"
#include "Haze/Objects/AssimpLoader.h"

// TEMP
#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace Haze 
{

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer() {

	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin() 
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() 
	{
		ImGuiIO& io = ImGui::GetIO();
		
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnImGuiRender() 
	{
		static bool show_model_loader = false;
		static bool show_program_loader = false;

		static auto func_model_loader = [](bool& show)
		{
			ImGui::Begin("Model Loader", &show);

			static std::array<char, 120> buffer;
			static Model* model = nullptr;

			ImGui::Text("Filepath:");	ImGui::SameLine();
			ImGui::InputText("##wid", buffer.data(), buffer.size() - 1);	ImGui::SameLine();
			
			if(ImGui::Button("Open"))
			{
				model = ModelLoader::Load(std::string(buffer.data()));
			}

			ImGui::Separator();

			if (model)
			{
				ImGui::Text("Triangles:");	ImGui::SameLine(200);	ImGui::Text("%d", []() -> unsigned int { unsigned int count = 0; for (auto m : model->Meshes) count += m->Triangles.size(); return count; }());
				ImGui::Text("Mesh count:");	ImGui::SameLine(200);	ImGui::Text("%d", model->Meshes.size());
				ImGui::Separator();

				if (ImGui::CollapsingHeader("Meshes")) 
				{
					for (unsigned int i = 0; i < model->Meshes.size(); i++) 
					{
						if (ImGui::TreeNode(std::string("Mesh " + std::to_string(i)).c_str()))
						{
							ImGui::Text("Vertices");	ImGui::SameLine(200);	ImGui::Text("%d", model->Meshes[i]->Vertices.size());
							ImGui::Text("Triangles");	ImGui::SameLine(200);	ImGui::Text("%d", model->Meshes[i]->Triangles.size());
							ImGui::TreePop();
						}
					}
				}
			} 
			else 
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No model opened!");
			}

			ImGui::End();
		};

		static auto func_program_loader = [](bool& show)
		{
			ImGui::Begin("Program Loader", &show);

			ImGui::End();
		};

		if (show_model_loader) func_model_loader(show_model_loader);
		if (show_program_loader) func_program_loader(show_program_loader);

		ImGui::Begin("Haze");

		if (ImGui::CollapsingHeader("Loaders"))
		{
			ImGui::Checkbox("Program", &show_program_loader);
			ImGui::Checkbox("Model", &show_model_loader);
		}

		ImGui::End();
	}


}