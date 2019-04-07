#include "hzpch.h"
#include "Program.h"

#include <glad/glad.h>

namespace Haze 
{

	Program::Program(const std::string& path, unsigned int type) 
	{
		_Handle = ProgramLoader::BuildProgram(path, type);
	}

	Program::~Program() 
	{
		glDeleteProgram(_Handle);
	}

	void Program::Bind() const 
	{
		glUseProgram(_Handle);
	}

	void Program::Unbind() const
	{
		glUseProgram(0);
	}

	void Program::SetUniformImpl(int loc, float a) { glUniform1f(loc, a); }
	void Program::SetUniformImpl(int loc, float a, float b) { glUniform2f(loc, a, b); }
	void Program::SetUniformImpl(int loc, float a, float b, float c) { glUniform3f(loc, a, b, c); }
	void Program::SetUniformImpl(int loc, float a, float b, float c, float d) { glUniform4f(loc, a, b, c, d); }
	void Program::SetUniformImpl(int loc, const glm::vec2& a) { glUniform2fv(loc, 1, &a[0]); }
	void Program::SetUniformImpl(int loc, const glm::vec3& a) { glUniform3fv(loc, 1, &a[0]); }
	void Program::SetUniformImpl(int loc, const glm::vec4& a) { glUniform4fv(loc, 1, &a[0]); }
	void Program::SetUniformImpl(int loc, bool a) { glUniform1i(loc, a); }
	void Program::SetUniformImpl(int loc, int a) { glUniform1i(loc, a); };
	void Program::SetUniformImpl(int loc, unsigned int a) { glUniform1i(loc, a); }
	void Program::SetUniformImpl(int loc, glm::mat4 a) { glUniformMatrix4fv(loc, 1, GL_FALSE, &a[0][0]); }

	int Program::GetUniformLocation(const std::string& name) {
		if (_LocationCache.find(name) == _LocationCache.end()) 
		{
			int location;

			if ((location = glGetUniformLocation(_Handle, name.c_str())) != -1)
			{
				_LocationCache[name] = location;
			}
			else
			{
				HZ_CORE_WARN("No uniform with name {0} does not exist!", name);
				return -1;
			}
		}

		return _LocationCache[name];
	}

	unsigned int ProgramLoader::GetType(unsigned int type) 
	{
		switch (type)
		{
			case ShaderType::ShaderTypeVertex: return GL_VERTEX_SHADER;
			case ShaderType::ShaderTypeFragment: return GL_FRAGMENT_SHADER;
			case ShaderType::ShaderTypeGeometry: return GL_GEOMETRY_SHADER;
			default: return -1;
		}
	}

	std::string ProgramLoader::GetPath(const std::string& path, unsigned int type) 
	{
		switch (type) 
		{
			case ShaderType::ShaderTypeVertex: return path + ".vert";
			case ShaderType::ShaderTypeFragment: return path + ".frag";
			case ShaderType::ShaderTypeGeometry: return path + ".geom";
			default: return path;
		}
	}

	std::string ProgramLoader::ReadFile(const std::string& filepath)
	{
		std::ifstream stream(filepath);

		if (!stream.is_open()) {
			HZ_CORE_ERROR("Shader not found!");
		}

		std::string token, line;
		std::ostringstream output;

		while (getline(stream, line))
		{
			std::istringstream input(line);

			if (input >> token) 
			{
				if (token.compare("#import") == 0) 
				{
					input >> token;
					output << ReadFile(filepath.substr(0, filepath.find_last_of('/') + 1) + token) << '\n';
				}
				else 
				{
					output << line << '\n';
				}
			} 
			else 
			{
				output << '\n';
			}
		}

		return output.str();
	}

	unsigned int ProgramLoader::BuildShader(const std::string& path, unsigned int shader) {
		unsigned int sid = glCreateShader(GetType(shader));

		std::string raw = ReadFile(GetPath(path, shader));
		const char* src = raw.c_str();

		glShaderSource(sid, 1, &src, nullptr);
		glCompileShader(sid);

		int glcs;
		glGetShaderiv(sid, GL_COMPILE_STATUS, &glcs);

		if (glcs == GL_FALSE) {
			int length;
			glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(sid, length, &length, message);

			HZ_CORE_WARN("Shader error: {0}", message);

			glDeleteShader(sid);

			return 0;
		}

		return sid;
	}

	unsigned int ProgramLoader::BuildProgram(const std::string& path, unsigned int shaders) {
		unsigned int hid = glCreateProgram();

		if (shaders & ShaderType::ShaderTypeFragment) glAttachShader(hid, BuildShader(path, ShaderType::ShaderTypeFragment));
		if (shaders & ShaderType::ShaderTypeVertex) glAttachShader(hid, BuildShader(path, ShaderType::ShaderTypeVertex));
		if (shaders & ShaderType::ShaderTypeGeometry) glAttachShader(hid, BuildShader(path, ShaderType::ShaderTypeGeometry));

		glLinkProgram(hid);
		glValidateProgram(hid);

		return hid;
	}

}