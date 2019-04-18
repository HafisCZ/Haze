#pragma once

#include "Haze/Core.h"
#include "Haze/Log.h"

#include <unordered_map>
#include <glm/glm.hpp>

namespace Haze
{

	enum class  ShaderType 
	{
		Fragment, Vertex, Geometry
	};

	class  Shader 
	{
		friend class Program;

		private:
			Shader() {}
			~Shader();
			
		public:
			static Shader* FromFile(const std::string& filepath, ShaderType type);
			static Shader* FromString(const std::string& shader, ShaderType type);

		private:
			static int GetType(ShaderType type);

		private:
			unsigned int _Handle;
	};

	class  Program 
	{
		public:
			Program(std::initializer_list<Shader*> shaders);
			~Program();

			void Bind() const;
			void Unbind() const;

			template <typename ... T> void SetUniform(const std::string& name, T&& ... t) { SetUniformImpl(GetUniformLocation(name), std::forward<T>(t)...); }

		private:
			int GetUniformLocation(const std::string& name);

			void SetUniformImpl(int location) {}
			void SetUniformImpl(int loc, float a);
			void SetUniformImpl(int loc, float a, float b);
			void SetUniformImpl(int loc, float a, float b, float c);
			void SetUniformImpl(int loc, float a, float b, float c, float d);
			void SetUniformImpl(int loc, const glm::vec2& a);
			void SetUniformImpl(int loc, const glm::vec3& a);
			void SetUniformImpl(int loc, const glm::vec4& a);
			void SetUniformImpl(int loc, bool a);
			void SetUniformImpl(int loc, int a);
			void SetUniformImpl(int loc, unsigned int a);
			void SetUniformImpl(int loc, glm::mat4 a);

		private:
			unsigned int _Handle;
			std::unordered_map<std::string, int> _Cache;
	};

}