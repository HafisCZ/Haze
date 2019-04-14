#include "hzpch.h"
#include "Program.h"

#include <glad/glad.h>

namespace Haze 
{

	Program::Program(std::initializer_list<Shader*> shaders) 
	{
		_Handle = glCreateProgram();

		for (auto shader : shaders) 
		{
			glAttachShader(_Handle, shader->_Handle);
		}

		glLinkProgram(_Handle);
		glValidateProgram(_Handle);
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
		if (_Cache.find(name) == _Cache.end())
		{
			int location;

			if ((location = glGetUniformLocation(_Handle, name.c_str())) != -1)
			{
				_Cache[name] = location;
			}
			else
			{
				HZ_CORE_WARN("No uniform with name {0} does not exist!", name);
				return -1;
			}
		}

		return _Cache[name];
	}

	Shader::~Shader() 
	{
		glDeleteShader(_Handle);
	}

	Shader* Shader::FromFile(const std::string& filepath, ShaderType type) {
		std::ifstream stream(filepath);

		if (!stream.is_open()) {
			HZ_CORE_ERROR("Shader not found!");
		}

		std::string token, line;
		std::ostringstream output;

		while (getline(stream, line)) {
			output << line << '\n';
		}

		return FromString(output.str(), type);
	}

	int Shader::GetType(ShaderType type) {
		switch (type) 
		{
			case ShaderType::Vertex: return GL_VERTEX_SHADER;
			case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
		}

		return -1;
	}

	Shader* Shader::FromString(const std::string& text, ShaderType type) {
		Shader* shader = new Shader();
		shader->_Handle = glCreateShader(GetType(type));

		const char* src = text.data();

		glShaderSource(shader->_Handle, 1, &src, nullptr);
		glCompileShader(shader->_Handle);

		int glcs;
		glGetShaderiv(shader->_Handle, GL_COMPILE_STATUS, &glcs);

		if (glcs == GL_FALSE) {
			int length;
			glGetShaderiv(shader->_Handle, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(shader->_Handle, length, &length, message);

			HZ_CORE_WARN("Shader error: {0}", message);

			glDeleteShader(shader->_Handle);

			return 0;
		}

		return shader;
	}

}