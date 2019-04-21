#pragma once

#include "Haze/Core.h"
#include "Haze/Objects/Model.h"
#include "Haze/Scene/Light.h"

#include "Haze/ImGui/Presets.h"
#include "Haze/Utils/Utils.h"

namespace Haze
{

	class Object
	{
		public:
			Object() {}
			Object(Model* model) : Model(model) {}

			~Object() { Repository::Unload(Model); }

			Model* Model;
			ModelMatrix Matrix;

			inline std::pair<Mesh*, float> IntersectsRay(glm::vec3 rayOrigin, glm::vec3 rayDir) {
				std::pair<Mesh*, float> ir = { nullptr, 0.0f };

				for (auto m : Model->Meshes) {
					auto mi = Math::Intersects_OBB_RAY(m->AABB_MIN, m->AABB_MAX, Matrix.GetScale(), Matrix.Matrix(), rayOrigin, rayDir);
					if (mi.first && (ir.first == nullptr || mi.second < ir.second)) {
						ir = { m, mi.second };
					}
				}

				return ir;
			}
	};

	class Scene
	{
		public:
			std::vector<Object*> Objects;

			std::vector<PointLight> Point;
			AmbientLight Ambient;
			VectorLight Vector;

			TextureCube* Skybox = nullptr;

			Object* Item = nullptr;
	};

}