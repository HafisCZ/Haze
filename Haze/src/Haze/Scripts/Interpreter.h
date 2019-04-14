#pragma once

#include "Haze/Core.h"

#include <vector>
#include <glm/glm.hpp>

namespace Haze 
{

	enum class Instruction : unsigned int {
		Error = 0,
		Camera, Scene,
		Object, Light, Point, Vector, Ambient, Model, Skybox,
		Set, Add, Reset, Remove, Load, Update,
		Yaw, Pitch, Position, Rotation, Scale
	};

	class Scene;
	class Camera;

	struct Command 
	{
		Command* Next;
		Instruction Inst;
		std::vector<std::string> Args;
		std::size_t ArgPtr;

		Command(Instruction inst, Command* next = nullptr) : Inst(inst), Next(next), ArgPtr(0) {}

		static bool Match(Command* node, std::vector<std::pair<Instruction, int>>& pattern) 
		{
			for (unsigned int i = 0; i < pattern.size(); node = node->Next, i++) if (!node || node->Inst != pattern[i].first || node->Args.size() != pattern[i].second) return false;
			return true;
		}

		inline float Float(unsigned int index = 0) {
			return (float) atof(Args[index % Args.size()].data());
		}

		inline glm::vec3 Vec(unsigned int index = 0) {
			return glm::vec3(Float(index), Float(index + 1), Float(index + 2));
		}

		inline const std::string& String(unsigned int index = 0) {
			return Args[index % Args.size()];
		}

		inline int Int(unsigned int index = 0) {
			return atoi(Args[index % Args.size()].data());
		}
	};

	struct HAZE_API Interpreter
	{
		static void ExecuteScript(char* script, Scene* scene, Camera* camera);
		static void TranslateScript(char* script, std::vector<Command*>& commands);

		static Instruction TranslateInstruction(const std::string& id);

		static void InvokeCommand(Command* command, Scene* scene, Camera* camera);
	};

}