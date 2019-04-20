#include "hzpch.h"
#include "UI.h"

#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Camera.h"

#include "imgui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Haze 
{
	void UI::ShowUI(Scene* scene, Camera* camera, Object* so, Mesh* sm)
	{
		ImVec2 wp = ImGui::GetWindowPos();
		ImGui::SetNextWindowPos({ wp.x - 60, wp.y - 40 }, ImGuiCond_Always);

		ImGui::Begin("UI", nullptr,
			ImGuiWindowFlags_NoBackground | 
			ImGuiWindowFlags_NoInputs | 
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDecoration | 
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoDocking | 
			ImGuiWindowFlags_AlwaysAutoResize
		);

		auto pos = camera->GetWorldPosition();
		auto dir = camera->GetDirection();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

		ImGui::SetCursorPos(ImVec2(20, 15));	ImGui::Text("XYZ");
		ImGui::SetCursorPos(ImVec2(80, 15));	ImGui::Text("%.2f", pos.x);
		ImGui::SetCursorPos(ImVec2(140, 15));	ImGui::Text("%.2f", pos.y);
		ImGui::SetCursorPos(ImVec2(200, 15));	ImGui::Text("%.2f", pos.z);	

		ImGui::SetCursorPos(ImVec2(20, 30));	ImGui::Text("DIR");
		ImGui::SetCursorPos(ImVec2(80, 30));	ImGui::Text("%.2f", dir.x);
		ImGui::SetCursorPos(ImVec2(140, 30));	ImGui::Text("%.2f", dir.y);
		ImGui::SetCursorPos(ImVec2(200, 30));	ImGui::Text("%.2f", dir.z);

		if (so) 
		{
			pos = so->Matrix.GetPosition();
			auto sca = so->Matrix.GetScale();
			auto rot = so->Matrix.GetRotation();

			ImGui::SetCursorPos(ImVec2(20, 60));	ImGui::Text("FP");
			ImGui::SetCursorPos(ImVec2(80, 60));	ImGui::Text("%s", so->Model->Name.data());

			ImGui::SetCursorPos(ImVec2(20, 75));	ImGui::Text("MC");
			ImGui::SetCursorPos(ImVec2(80, 75));	ImGui::Text("%d", so->Model->Meshes.size());

			ImGui::SetCursorPos(ImVec2(20, 90));	ImGui::Text("V");
			ImGui::SetCursorPos(ImVec2(80, 90));	ImGui::Text("%d", so->Model->Vertices);
			ImGui::SetCursorPos(ImVec2(140, 90));	ImGui::Text("T");
			ImGui::SetCursorPos(ImVec2(200, 90));	ImGui::Text("%d", so->Model->Triangles);

			ImGui::SetCursorPos(ImVec2(20, 105));	ImGui::Text("P");
			ImGui::SetCursorPos(ImVec2(80, 105));	ImGui::Text("%.2f", pos.x);
			ImGui::SetCursorPos(ImVec2(140, 105));	ImGui::Text("%.2f", pos.y);
			ImGui::SetCursorPos(ImVec2(200, 105));	ImGui::Text("%.2f", pos.z);

			ImGui::SetCursorPos(ImVec2(20, 120));	ImGui::Text("R");
			ImGui::SetCursorPos(ImVec2(80, 120));	ImGui::Text("%.2f", rot.x);
			ImGui::SetCursorPos(ImVec2(140, 120));	ImGui::Text("%.2f", rot.y);
			ImGui::SetCursorPos(ImVec2(200, 120));	ImGui::Text("%.2f", rot.z);

			ImGui::SetCursorPos(ImVec2(20, 135));	ImGui::Text("S");
			ImGui::SetCursorPos(ImVec2(80, 135));	ImGui::Text("%.2f", sca.x);
			ImGui::SetCursorPos(ImVec2(140, 135));	ImGui::Text("%.2f", sca.y);
			ImGui::SetCursorPos(ImVec2(200, 135));	ImGui::Text("%.2f", sca.z);

			if (sm)
			{
				ImGui::SetCursorPos(ImVec2(20, 165));	ImGui::Text("V");
				ImGui::SetCursorPos(ImVec2(80, 165));	ImGui::Text("%d", sm->Vertices.size());
				ImGui::SetCursorPos(ImVec2(140, 165));	ImGui::Text("T");
				ImGui::SetCursorPos(ImVec2(200, 165));	ImGui::Text("%d", sm->Triangles.size());

				ImGui::SetCursorPos(ImVec2(20, 180));	ImGui::Text("Bm");
				ImGui::SetCursorPos(ImVec2(80, 180));	ImGui::Text("%.2f", sm->AABB_MIN.x);
				ImGui::SetCursorPos(ImVec2(140, 180));	ImGui::Text("%.2f", sm->AABB_MIN.y);
				ImGui::SetCursorPos(ImVec2(200, 180));	ImGui::Text("%.2f", sm->AABB_MIN.z);

				ImGui::SetCursorPos(ImVec2(20, 195));	ImGui::Text("BM");
				ImGui::SetCursorPos(ImVec2(80, 195));	ImGui::Text("%.2f", sm->AABB_MAX.x);
				ImGui::SetCursorPos(ImVec2(140, 195));	ImGui::Text("%.2f", sm->AABB_MAX.y);
				ImGui::SetCursorPos(ImVec2(200, 195));	ImGui::Text("%.2f", sm->AABB_MAX.z);

				ImGui::SetCursorPos(ImVec2(20, 210));	ImGui::Text("Td");
				ImGui::SetCursorPos(ImVec2(80, 210));	ImGui::Text("%s", sm->Textures[0] ? "true" : "false");
				ImGui::SetCursorPos(ImVec2(140, 210));	ImGui::Text("Tn");
				ImGui::SetCursorPos(ImVec2(200, 210));	ImGui::Text("%s", sm->Textures[1] ? "true" : "false");
				ImGui::SetCursorPos(ImVec2(20, 225));	ImGui::Text("Ts");
				ImGui::SetCursorPos(ImVec2(80, 225));	ImGui::Text("%s", sm->Textures[2] ? "true" : "false");
				ImGui::SetCursorPos(ImVec2(140, 225));	ImGui::Text("Ta");
				ImGui::SetCursorPos(ImVec2(200, 225));	ImGui::Text("%s", sm->Textures[3] ? "true" : "false");
			}
		}

		ImGui::PopStyleColor();
		ImGui::End();

	}


}