#pragma once

#include "Haze/Core.h"
#include "Haze/Log.h"

#include <unordered_map>
#include <glm/glm.hpp>

namespace Haze
{

	enum ShaderType 
	{
		ShaderTypeVertex		= BIT(0),
		ShaderTypeFragment		= BIT(1),
		ShaderTypeGeometry		= BIT(2)
	};

	class HAZE_API Program 
	{
		friend class ProgramLoader;

		public:
			Program(const std::string& path, unsigned int type);
			~Program();

			void Bind() const;
			void Unbind() const;

			template <typename ... T> void SetUniform(const std::string& name, T&& ... t) { SetUniformImpl(GetUniformLocation(name), std::forward<T>(t)...); }

			int GetUniformLocation(const std::string& name);

		private:
			inline void SetUniformImpl(int location) { }
			inline void SetUniformImpl(int loc, float a);
			inline void SetUniformImpl(int loc, float a, float b);
			inline void SetUniformImpl(int loc, float a, float b, float c);
			inline void SetUniformImpl(int loc, float a, float b, float c, float d);
			inline void SetUniformImpl(int loc, const glm::vec2& a);
			inline void SetUniformImpl(int loc, const glm::vec3& a);
			inline void SetUniformImpl(int loc, const glm::vec4& a);
			inline void SetUniformImpl(int loc, bool a);
			inline void SetUniformImpl(int loc, int a);
			inline void SetUniformImpl(int loc, unsigned int a);
			inline void SetUniformImpl(int loc, glm::mat4 a);

		private:
			unsigned int _Handle;

			std::unordered_map<std::string, unsigned int> _LocationCache;
	};

	class ProgramLoader 
	{
		public:
			static unsigned int GetType(unsigned int type);

			static std::string GetPath(const std::string& path, unsigned int type);
			static std::string ReadFile(const std::string& filepath);

			static unsigned int BuildShader(const std::string& path, unsigned int shader);

			static unsigned int BuildProgram(const std::string& path, unsigned int shaders);
	};

}