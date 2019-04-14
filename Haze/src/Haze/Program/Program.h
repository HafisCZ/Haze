#pragma once

#include "Haze/Core.h"
#include "Haze/Log.h"

#include <unordered_map>
#include <glm/glm.hpp>

namespace Haze
{

	enum class HAZE_API ShaderType 
	{
		Fragment, Vertex, Geometry
	};

	class HAZE_API Shader 
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

	class HAZE_API Program 
	{
		public:
			Program(std::initializer_list<Shader*> shaders);
			~Program();

			void Bind() const;
			void Unbind() const;

			template <typename ... T> void SetUniform(const std::string& name, T&& ... t) { SetUniformImpl(GetUniformLocation(name), std::forward<T>(t)...); }

		private:
			int GetUniformLocation(const std::string& name);

			inline void SetUniformImpl(int location) {}
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
			std::unordered_map<std::string, int> _Cache;
	};

}