#include "hzpch.h"
#include "ScriptReader.h"

#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Camera.h"

namespace Haze
{

	struct Command {
		virtual void Execute(Scene*& scene, Camera*& camera) {}
	};

	struct Command_System_Error : public Command {
		virtual void Execute(Scene*& scene, Camera*& camera) override {
			HZ_CORE_ERROR("UNDEFINED SYNTAX!");
		}
	};

	struct Command_Scene_Clear : public Command {
		virtual void Execute(Scene*& scene, Camera*& camera) override {
			delete camera;
			delete scene;

			scene = new Scene();
			camera = new Camera();
		}
	};

	struct RawCommand 
	{
		std::vector<std::string> items;
		std::vector<char> delim;
	};

	struct ScriptAnalyzer 
	{
		static void Analyze(char* script, std::vector<RawCommand>& commands)
		{ 
			static char* TOKEN_DELIMITERS = "\0\n()[],;\\/";
			static auto IS_DELIMITER = [](char c, char* delims) { for (unsigned int i = 0; i < strnlen_s(delims, 20); i++) if (c == delims[i]) return true; return false; };

			std::stringstream stream;
			RawCommand current;

			for (unsigned int i = 0; i < strnlen_s(script, 1000); i++) {
				char& c = script[i];

				if (c == ';') {
					if (stream.str().size() != 0) current.items.push_back(stream.str());
					commands.push_back(current);

					current = RawCommand();
					
					stream.str(std::string());
				} else if (IS_DELIMITER(c, " ()[]:.{}\?\\/\'\"")) {
					if (stream.str().size() != 0) current.items.push_back(stream.str());
					current.delim.push_back(c);

					stream.str(std::string());
				} else if (c == '\n') {
					continue;
				} else {
					stream.put(c);
				}
			}
		}

		static Command* Tokenize(RawCommand cmd) {
			if (cmd.delim.size() == 3 && cmd.delim[0] == '.' && cmd.delim[1] == '(' && cmd.delim[2] == ')') {
				if (cmd.items.size() == 2 && cmd.items[0] == "scene" && cmd.items[1] == "clear") return new Command_Scene_Clear();
			}

			return new Command_System_Error();
		}
	};
	
	void ScriptReader::Execute(char* script, Scene*& scene, Camera*& camera) 
	{
		std::vector<RawCommand> raws;
		ScriptAnalyzer::Analyze(script, raws);
		for (auto raw : raws) {
			auto cmd = ScriptAnalyzer::Tokenize(raw);

			cmd->Execute(scene, camera);
			delete cmd;
		}
	}

}