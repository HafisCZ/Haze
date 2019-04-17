#include "hzpch.h"
#include "UI.h"

#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Camera.h"

#include "imgui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Haze 
{

	int UI::GetAverageFps() {
		static double last = glfwGetTime();
		static double samples[120];
		static int samples_cnt = 0;

		double time = glfwGetTime();
		samples[samples_cnt++ % 120] = time - last;
		last = time;

		double average = 0.0;
		for (int i = 0; i < 120; i++) {
			average += samples[i];
		}

		return (int) (1.0 / (average / 120.0));
	}

	void UI::ShowUI(Scene* scene, Camera* camera, std::pair<float, unsigned int> raw) 
	{
		ImGui::Begin("UI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowPos("UI", ImGui::GetWindowPos());

		auto pos = camera->GetWorldPosition();
		auto dir = camera->GetDirection();
		auto[lok, id] = camera->GetWorldPointer(raw);
		auto fps = GetAverageFps();

		ImGui::SetCursorPos(ImVec2(20, 20));	ImGui::Text("FPS");
		ImGui::SetCursorPos(ImVec2(20, 40));	ImGui::Text("XYZ");
		ImGui::SetCursorPos(ImVec2(20, 60));	ImGui::Text("DIR");

		ImGui::SetCursorPos(ImVec2(20, 100));	ImGui::Text("TR");
		ImGui::SetCursorPos(ImVec2(20, 120));	ImGui::Text("ID");

		ImGui::SetCursorPos(ImVec2(80, 20));	ImGui::Text("%d", fps);
		
		ImGui::SetCursorPos(ImVec2(80, 40));	ImGui::Text("%.2f", pos.x);
		ImGui::SetCursorPos(ImVec2(130, 40));	ImGui::Text("%.2f", pos.y);
		ImGui::SetCursorPos(ImVec2(180, 40));	ImGui::Text("%.2f", pos.z);	
		
		ImGui::SetCursorPos(ImVec2(80, 60));	ImGui::Text("%.2f", dir.x);
		ImGui::SetCursorPos(ImVec2(130, 60));	ImGui::Text("%.2f", dir.y);
		ImGui::SetCursorPos(ImVec2(180, 60));	ImGui::Text("%.2f", dir.z);

		ImGui::SetCursorPos(ImVec2(80, 100));	ImGui::Text("%.2f", lok.x);
		ImGui::SetCursorPos(ImVec2(130, 100));	ImGui::Text("%.2f", lok.y);
		ImGui::SetCursorPos(ImVec2(180, 100));	ImGui::Text("%.2f", lok.z);

		ImGui::SetCursorPos(ImVec2(80, 120));	ImGui::Text("%d", id);

		if (id > 0) 
		{
			auto obj = scene->Objects[id - 1];

			ImGui::SetCursorPos(ImVec2(20, 180));	ImGui::Text("FP");
			ImGui::SetCursorPos(ImVec2(20, 200));	ImGui::Text("MC");
			ImGui::SetCursorPos(ImVec2(20, 220));	ImGui::Text("V");
			ImGui::SetCursorPos(ImVec2(20, 240));	ImGui::Text("T");

			ImGui::SetCursorPos(ImVec2(80, 180));	ImGui::Text("%s", obj->Model->Name.data());
			ImGui::SetCursorPos(ImVec2(80, 200));	ImGui::Text("%d", obj->Model->Meshes.size());
			ImGui::SetCursorPos(ImVec2(80, 220));	ImGui::Text("%d", obj->Model->Vertices);
			ImGui::SetCursorPos(ImVec2(80, 240));	ImGui::Text("%d", obj->Model->Triangles);
		}

		ImGui::End();
	}


}