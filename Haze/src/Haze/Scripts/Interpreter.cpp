#include "hzpch.h"
#include "Interpreter.h"

#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Camera.h"

#include <iomanip>
#include <regex>

namespace Haze
{

	void Interpreter::ExecuteScript(char* script, Scene* scene, Camera* camera) {
		std::vector<Command*> commands;
		Interpreter::TranslateScript(script, commands);

		for (auto command : commands) {
			Interpreter::InvokeCommand(command, scene, camera);
		}
	}

	void Interpreter::Dump(std::array<char, 5000>& out, Scene* scene, Camera* camera)
	{
		std::stringstream stream;

		stream << std::setprecision(3);

		stream << "light reset;\n";
		stream << "object reset;\n";
		stream << "camera set(" << camera->_WorldPosition.x << "," << camera->_WorldPosition.y << "," << camera->_WorldPosition.z << "," << camera->_Yaw << "," << camera->_Pitch << "," << camera->_Fov << ");\n";

		for (int i = 0; i < scene->Objects.size(); i++) {
			auto obj = scene->Objects[i];
			stream << "object add(" << obj->Model->Name << ");\n";
			stream << "object(" << i << ") position(" << obj->Matrix._Position.x << "," << obj->Matrix._Position.y << "," << obj->Matrix._Position.z << ");\n";
			stream << "object(" << i << ") rotation(" << obj->Matrix._Rotation.x << "," << obj->Matrix._Rotation.y << "," << obj->Matrix._Rotation.z << ");\n";
			stream << "object(" << i << ") scale(" << obj->Matrix._Scale.x << "," << obj->Matrix._Scale.y << "," << obj->Matrix._Scale.z << ");\n";
		}

		stream << "light ambient(" << scene->Ambient.GetData()[0].x << "," << scene->Ambient.GetData()[0].y << "," << scene->Ambient.GetData()[0].z << "," << scene->Ambient.GetData()[2].x << ");\n";
		stream << "light vector(" << scene->Vector.GetData()[0].x << "," << scene->Vector.GetData()[0].y << "," << scene->Vector.GetData()[0].z << "," << scene->Vector.GetData()[1].x << "," << scene->Vector.GetData()[1].y << "," << scene->Vector.GetData()[1].z << "," << scene->Vector.GetData()[2].y << scene->Vector.GetData()[2].z << ");\n";

		for (int i = 0; i < scene->Point.size(); i++) {
			auto l = scene->Point[i];
			stream << "light add(" <<
				l.GetData()[0].x << "," << l.GetData()[0].y << "," << l.GetData()[0].z << "," <<
				l.GetData()[1].x << "," << l.GetData()[1].y << "," << l.GetData()[1].z << "," <<
				l.GetData()[2].y << "," << l.GetData()[2].z << "," <<
				l.GetData()[3].y << "," << l.GetData()[3].z << "," <<  (l.IsCastingShadow() ? "1" : "0") << ");\n";
		}

		out.fill(0);

		unsigned int pos = 0;
		for (auto c : stream.str()) 
		{
			if (pos >= out.size()) break;
			out[pos++] = c;
		}
	}

	void Interpreter::Dump(const std::string& out, Scene* scene, Camera* camera) {
		std::stringstream stream;

		stream << std::setprecision(3);

		stream << "light reset;\n";
		stream << "object reset;\n";
		stream << "camera set(" << camera->_WorldPosition.x << "," << camera->_WorldPosition.y << "," << camera->_WorldPosition.z << "," << camera->_Yaw << "," << camera->_Pitch << "," << camera->_Fov << ");\n";

		for (int i = 0; i < scene->Objects.size(); i++) {
			auto obj = scene->Objects[i];
			stream << "object add(" << obj->Model->Name << ");\n";
			stream << "object(" << i << ") position(" << obj->Matrix._Position.x << "," << obj->Matrix._Position.y << "," << obj->Matrix._Position.z << ");\n";
			stream << "object(" << i << ") rotation(" << obj->Matrix._Rotation.x << "," << obj->Matrix._Rotation.y << "," << obj->Matrix._Rotation.z << ");\n";
			stream << "object(" << i << ") scale(" << obj->Matrix._Scale.x << "," << obj->Matrix._Scale.y << "," << obj->Matrix._Scale.z << ");\n";
		}

		stream << "light ambient(" << scene->Ambient.GetData()[0].x << "," << scene->Ambient.GetData()[0].y << "," << scene->Ambient.GetData()[0].z << "," << scene->Ambient.GetData()[2].x << ");\n";
		stream << "light vector(" << scene->Vector.GetData()[0].x << "," << scene->Vector.GetData()[0].y << "," << scene->Vector.GetData()[0].z << "," << scene->Vector.GetData()[1].x << "," << scene->Vector.GetData()[1].y << "," << scene->Vector.GetData()[1].z << "," << scene->Vector.GetData()[2].y << scene->Vector.GetData()[2].z << ");\n";

		for (int i = 0; i < scene->Point.size(); i++) {
			auto l = scene->Point[i];
			stream << "light add(" <<
				l.GetData()[0].x << "," << l.GetData()[0].y << "," << l.GetData()[0].z << "," <<
				l.GetData()[1].x << "," << l.GetData()[1].y << "," << l.GetData()[1].z << "," <<
				l.GetData()[2].y << "," << l.GetData()[2].z << "," <<
				l.GetData()[3].y << "," << l.GetData()[3].z << "," << (l.IsCastingShadow() ? "1" : "0") << ");\n";
		}

		std::fstream ostr(out, std::ios::trunc | std::ios::out);
		ostr << stream.str();
	}

	void Interpreter::TranslateScript(char* sscript, std::vector<Command*>& commands) {
		std::string script(sscript);
		std::transform(script.begin(), script.end(), script.begin(), ::tolower);

		std::sregex_token_iterator iterator, end;
		std::regex regex_semicolumn("[;]+");
		std::regex regex_spacecolumn("[,]+");
		std::regex regex_brackets("[\\(\\)]+");
		std::regex regex_whitespace("[\\s]+");

		std::vector<std::string> raw_commands;
		for (iterator = std::sregex_token_iterator(script.begin(), script.end(), regex_semicolumn, -1); iterator != end; iterator++) {
			if (!iterator->str().empty()) {
				raw_commands.emplace_back(iterator->str());
			}
		}

		auto split = [&](std::string& raw, std::regex regex) {
			std::vector<std::string> values;

			for (std::sregex_token_iterator split_iterator(raw.begin(), raw.end(), regex, -1); split_iterator != end; split_iterator++) {
				if (!split_iterator->str().empty()) {
					values.emplace_back(split_iterator->str());
				}
			}

			return values;
		};

		for (auto raw_command : raw_commands) {
			Command* root = nullptr;
			Command* node = nullptr;

			bool write_args = false;

			for (iterator = std::sregex_token_iterator(raw_command.begin(), raw_command.end(), regex_brackets, -1); iterator != end; iterator++, write_args ^= true) {
				if (write_args) {
					for (auto value : split(iterator->str(), regex_spacecolumn)) {
						node->Args.emplace_back(value);
					}
				} else {
					for (auto value : split(iterator->str(), regex_whitespace)) {
						if (node) {
							node->Next = new Command(Interpreter::TranslateInstruction(value));
							node = node->Next;
						} else {
							node = root = new Command(Interpreter::TranslateInstruction(value));
						}
					}
				}
			}

			commands.push_back(root);
		}
	}

	Instruction Interpreter::TranslateInstruction(const std::string& id) {
		static std::unordered_map<std::string, Instruction> lookup;
		static bool init = []() {
			lookup["camera"] = Instruction::Camera;
			lookup["scene"] = Instruction::Scene;

			lookup["object"] = Instruction::Object;
			lookup["light"] = Instruction::Light;
			lookup["point"] = Instruction::Point;
			lookup["vector"] = Instruction::Vector;
			lookup["ambient"] = Instruction::Ambient;
			lookup["model"] = Instruction::Model;
			lookup["skybox"] = Instruction::Skybox;

			lookup["set"] = Instruction::Set;
			lookup["add"] = Instruction::Add;
			lookup["reset"] = Instruction::Reset;
			lookup["remove"] = Instruction::Remove;
			lookup["load"] = Instruction::Load;
			lookup["update"] = Instruction::Update;

			lookup["yaw"] = Instruction::Yaw;
			lookup["pitch"] = Instruction::Pitch;
			lookup["position"] = Instruction::Position;
			lookup["rotation"] = Instruction::Rotation;
			lookup["scale"] = Instruction::Scale;

			return true;
		}();

		if (lookup.find(id) != lookup.end()) {
			return lookup[id];
		} else {
			return Instruction::Error;
		}
	}

	void Interpreter::InvokeCommand(Command* cmd, Scene* scene, Camera* camera) {
		using OP = Haze::Instruction;
		using CheckVector = std::vector<std::pair<Instruction, int>>;
		
		static CheckVector CAMERA_SET{ {OP::Camera, 0}, {OP::Set, 6} };

		static CheckVector OBJECT_ADD{ {OP::Object, 0}, {OP::Add, 1} };
		static CheckVector OBJECT_RESET{ {OP::Object, 0}, {OP::Reset, 0} };
		static CheckVector OBJECT_SELECT{ {OP::Object, 1} };

		static CheckVector LIGHT_VECTOR{ {OP::Light, 0}, {OP::Vector, 8} };
		static CheckVector LIGHT_AMBIENT{ {OP::Light, 0},{OP::Ambient, 4} };
		static CheckVector LIGHT_ADD{ {OP::Light, 0}, {OP::Add, 11} };
		static CheckVector LIGHT_RESET{ {OP::Light, 0}, {OP::Reset, 0} };
		static CheckVector LIGHT_SELECT{ {OP::Light,1} };

		static CheckVector SKYBOX_SET{ {OP::Skybox, 0}, {OP::Set, 1} };
		static CheckVector SKYBOX_REMOVE{ {OP::Skybox, 0}, {OP::Remove, 0} };

		static CheckVector REMOVE{ {OP::Remove, 0} };
		static CheckVector POSITION{ { OP::Position, 3 } };
		static CheckVector SCALE{ { OP::Scale, 3 } };
		static CheckVector ROTATION{ { OP::Rotation, 3 } };

		static CheckVector SET_9{ {OP::Set, 9} };
		static CheckVector SET_11{ {OP::Set, 11} };

		if (Command::Match(cmd, CAMERA_SET)) 
		{
			Command* sub = cmd->Next;
			
			camera->_WorldPosition = sub->Vec();
			camera->_Yaw = sub->Float(3);
			camera->_Pitch = sub->Float(4);
			camera->_Fov = sub->Float(5);
			camera->UpdateMatrices();
		} 
		else if (Command::Match(cmd, OBJECT_ADD)) 
		{
			Command* sub = cmd->Next;

			Model* model = ModelLoader::Load(sub->String());
			if (model) 
			{
				scene->Objects.push_back(new Object(model));
			}
		}
		else if (Command::Match(cmd, OBJECT_RESET)) 
		{
			scene->Objects.clear();
		}
		else if (Command::Match(cmd, OBJECT_SELECT)) 
		{
			Command* sub = cmd->Next;

			int index = cmd->Int();

			if (Command::Match(sub, REMOVE)) 
			{
				scene->Objects.erase(scene->Objects.begin() + index);
			} 
			else if (Command::Match(sub, POSITION)) 
			{
				scene->Objects[index]->Matrix.SetPosition(sub->Vec());
			}
			else if (Command::Match(sub, ROTATION)) 				
			{
				scene->Objects[index]->Matrix.SetRotation(sub->Vec());
			}
			else if (Command::Match(sub, SCALE)) 
			{
				scene->Objects[index]->Matrix.SetScale(sub->Vec());
			}
			else if (Command::Match(sub, SET_9)) 
			{
				scene->Objects[index]->Matrix.SetPosition(sub->Vec());
				scene->Objects[index]->Matrix.SetRotation(sub->Vec(3));
				scene->Objects[index]->Matrix.SetScale(sub->Vec(6));
			}
		} 
		else if (Command::Match(cmd, LIGHT_VECTOR)) 
		{
			Command* sub = cmd->Next;
		
			scene->Vector.Set(sub->Vec(), sub->Vec(3), glm::vec3(0.0f, sub->Float(6), sub->Float(7)), glm::vec3(0.0f), false);
		}
		else if (Command::Match(cmd, LIGHT_AMBIENT)) 
		{
			Command* sub = cmd->Next;

			scene->Ambient.Set(sub->Vec(), glm::vec3(0.0f), glm::vec3(sub->Float(3), 0.0f, 0.0f), glm::vec3(0.0f), false);
		}
		else if (Command::Match(cmd, LIGHT_RESET)) 
		{
			scene->Ambient.Reset();
			scene->Vector.Reset();
			scene->Point.clear();
		}
		else if (Command::Match(cmd, LIGHT_ADD)) 
		{
			Command* sub = cmd->Next;

			scene->Point.emplace_back(sub->Vec(), sub->Vec(3), sub->Float(6), sub->Float(7), sub->Float(8), sub->Float(9), sub->Int(10));
		}
		else if (Command::Match(cmd, LIGHT_SELECT)) 
		{
			Command* sub = cmd->Next;

			int index = cmd->Int();

			if (Command::Match(sub, SET_11)) 
			{
				scene->Point[index].Set(sub->Vec(), sub->Vec(3), glm::vec3(0.0f, sub->Float(6), sub->Float(7)), glm::vec3(1.0f, sub->Float(8), sub->Float(9)), sub->Int(10));
			}
			else if (Command::Match(sub, REMOVE)) {
				scene->Point.erase(scene->Point.begin() + index);
			}
		}
		else if (Command::Match(cmd, SKYBOX_SET))
		{
			Command* sub = cmd->Next;

			TextureCube* skybox = static_cast<TextureCube*>(TextureLoader::LoadCube(sub->String()));
			if (skybox) {
				scene->Skybox = skybox;
			}
		}
		else if (Command::Match(cmd, SKYBOX_REMOVE))
		{ 
			scene->Skybox = nullptr;
		}
	}
}