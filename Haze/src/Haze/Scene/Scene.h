#pragma once

#include "Haze/Core.h"
#include "Haze/Objects/Model.h"
#include "Haze/Scene/Light.h"

#include "Haze/ImGui/Presets.h"

#include <glm/glm.hpp>

namespace Haze
{

	class HAZE_API ModelMatrix
	{
		friend struct Interpreter;
		friend struct GUI;

		public:
			inline operator const glm::mat4& () { UpdateMatrices(); return _TempMatrix; }
			inline const glm::mat4& Inverse() { UpdateMatrices(); return _TempMatrixI; }

			inline const glm::vec3& GetRotation() const { return _Rotation; }
			inline const glm::vec3& GetScale() const { return _Scale; }
			inline const glm::vec3& GetPosition() const { return _Position; }

			void SetRotation(glm::vec3 rotation) { _Rotation = rotation; _Invalid = true; }
			void SetScale(glm::vec3 scale) { _Scale = scale; _Invalid = true; }
			void SetPosition(glm::vec3 position) { _Position = position; _Invalid = true; }

		private:
			void UpdateMatrices(bool force = false);

		private:
			bool _Invalid = false;
			
			glm::vec3 _Scale = glm::vec3(1.0f);
			glm::vec3 _Position = glm::vec3(0.0f);
			glm::vec3 _Rotation = glm::vec3(0.0f);

			glm::mat4 _TempMatrix = glm::mat4(1.0f);
			glm::mat4 _TempMatrixI = glm::mat4(1.0f);
	};

	class HAZE_API Object
	{
		public:
			Object() {}
			Object(Model* model) : Model(model) {}

			Model* Model;
			ModelMatrix Matrix;
	};
	
	class HAZE_API Scene
	{
		private:
			struct LightContainer
			{
				Light* Ambient = new AmbientLight(glm::vec3(1.0f), 0.1f);
				Light* Vector = new VectorLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.1f, 0.1f);
				std::vector<Light*> Point;

				~LightContainer() {
					delete Ambient;
					delete Vector;
					for (auto m : Point) {
						delete m;
					}
				}
			};

		public:
			~Scene() {
				delete Lights;
				delete Skybox;
				for (auto o : Objects) {
					delete o;
				}
			}

			std::vector<Object*> Objects;
			LightContainer* Lights = new LightContainer();
			TextureCube* Skybox;

			Object* item = nullptr;
	};

}