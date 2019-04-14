#include "hzpch.h"
#include "ProgramAdapter.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

namespace Haze 
{

	void GeometryPassAdapter::Set(Scene* scene, Camera* camera) {
		_Program->SetUniform("uProjectionMatrix", camera->GetProjectionMatrix());
		_Program->SetUniform("uViewMatrix", camera->GetViewMatrix());
	}

	void GeometryPassAdapter::Set(Object* object, Camera* camera) {
		_Program->SetUniform("uModelMatrix", object->Matrix.Matrix());
		_Program->SetUniform("uModelMatrixI", object->Matrix.Inverse());
	}

	void GeometryPassAdapter::Set(Mesh* mesh, Camera* camera) {
		_Program->SetUniform("uTexture[0]", mesh->Textures[0] != nullptr);
		_Program->SetUniform("uTexture[1]", mesh->Textures[1] != nullptr);
		_Program->SetUniform("uTexture[2]", mesh->Textures[2] != nullptr);

		_Program->SetUniform("uTexture0", TextureAllocator::Bind(mesh->Textures[0]));
		_Program->SetUniform("uTexture1", TextureAllocator::Bind(mesh->Textures[1]));
		_Program->SetUniform("uTexture2", TextureAllocator::Bind(mesh->Textures[2]));
	}

	void SkyboxAdapter::Set(Scene* scene, Camera* camera) {
		glDepthFunc(GL_LEQUAL);

		_Program->SetUniform("uTranslatedViewMatrix", glm::mat4(glm::mat3(camera->GetViewMatrix())));
		_Program->SetUniform("uProjectionMatrix", camera->GetProjectionMatrix());
		_Program->SetUniform("uTextureCube", TextureAllocator::Bind(scene->Skybox));
	}

	void SkyboxAdapter::OnFinish() {
		glDepthFunc(GL_LESS);
	}

	void SkyboxAdapter::Set(Object* object, Camera* camera) {

	}

	void SkyboxAdapter::Set(Mesh* mesh, Camera* camera) {

	}

	void ForwardPassAdapter::Set(Scene* scene, Camera* camera) {

	}

	void ForwardPassAdapter::Set(Object* object, Camera* camera) {
		_Program->SetUniform("uModelViewProjectionMatrix", camera->GetProjectionMatrix() * camera->GetViewMatrix() * object->Matrix.Matrix());
		_Program->SetUniform("uColor", 0.5f, 0.5f, 0.5f);
	}

	void ForwardPassAdapter::Set(Mesh* mesh, Camera* camera) {
		_Program->SetUniform("uTexture[0]", mesh->Textures[0] != nullptr);
		_Program->SetUniform("uTexture[1]", mesh->Textures[1] != nullptr);
		_Program->SetUniform("uTexture[2]", mesh->Textures[2] != nullptr);

		_Program->SetUniform("uTexture0", TextureAllocator::Bind(mesh->Textures[0]));
		_Program->SetUniform("uTexture1", TextureAllocator::Bind(mesh->Textures[1]));
		_Program->SetUniform("uTexture2", TextureAllocator::Bind(mesh->Textures[2]));
	}

	void LightingPassAdapter::Set(Scene* scene, Camera* camera) {
		_Program->SetUniform("uPosition", 0);
		_Program->SetUniform("uNormal", 1);
		_Program->SetUniform("uAlbedoSpecular", 2);
		_Program->SetUniform("uPointShadowMap", 3);

		unsigned int index = 0;

		for (unsigned int i = 0; i < scene->Point.size(); i++) {
			Light& light = scene->Point[i];

			_Program->SetUniform("uLight[" + std::to_string(index) + "].Position", light.GetData()[1]);
			_Program->SetUniform("uLight[" + std::to_string(index) + "].Color", light.GetData()[0]);

			_Program->SetUniform("uLight[" + std::to_string(index) + "].Diffuse", light.GetData()[2].y);
			_Program->SetUniform("uLight[" + std::to_string(index) + "].Specular", light.GetData()[2].z);

			const float constant = light.GetData()[3].x;
			const float linear = light.GetData()[3].y;
			const float quadratic = light.GetData()[3].z;

			_Program->SetUniform("uLight[" + std::to_string(index) + "].Linear", linear);
			_Program->SetUniform("uLight[" + std::to_string(index) + "].Quadratic", quadratic);

			const float maxBrightness = std::fmaxf(std::fmaxf(light.GetData()[0].r, light.GetData()[0].g), light.GetData()[0].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);

			_Program->SetUniform("uLight[" + std::to_string(index) + "].Radius", radius);
			_Program->SetUniform("uLight[" + std::to_string(index) + "].Shadow", light.IsCastingShadow());

			index++;
		}

		_Program->SetUniform("uVectorLight.Enabled", true);
		_Program->SetUniform("uVectorLight.Color", scene->Vector.GetData()[0]);
		_Program->SetUniform("uVectorLight.Direction", scene->Vector.GetData()[1]);
		_Program->SetUniform("uVectorLight.Intensity", scene->Vector.GetData()[2]);

		_Program->SetUniform("uAmbientLight.Enabled", true);
		_Program->SetUniform("uAmbientLight.Color", scene->Ambient.GetData()[0]);
		_Program->SetUniform("uAmbientLight.Intensity", scene->Ambient.GetData()[2]);

		_Program->SetUniform("uLightCount", index);
		_Program->SetUniform("uCameraPosition", camera->GetWorldPosition());
	}

	void LightingPassAdapter::Set(Object* object, Camera* camera) {

	}

	void LightingPassAdapter::Set(Mesh* mesh, Camera* camera) {

	}

	void ShadingPassAdapter::Set(Scene* scene, Camera* camera) {
		static glm::mat4 perspective = glm::perspective(glm::half_pi<float>(), 1.0f, 1.0f, 100.0f);
		static glm::vec3 vectors[] = {
			{ 1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }
		};

		unsigned int index = 0;
		for (auto& light : scene->Point) {
			if (!light.IsCastingShadow()) {
				continue;
			}

			_Program->SetUniform("uPosition[" + std::to_string(index) + "]", light.GetData()[1]);

			for (unsigned int i = 0; i < 6; i++) {
				_Program->SetUniform("uFace[" + std::to_string(index * 6 + i) + "]", perspective * glm::lookAt(light.GetData()[1], light.GetData()[1] + vectors[i], i == 2 ? vectors[4] : (i == 3 ? vectors[5] : vectors[3])));
			}

			if (++index > 5) {
				break;
			}
		}

		_Program->SetUniform("uFaceCount", index * 6);
	}

	void ShadingPassAdapter::Set(Object* object, Camera* camera) {
		_Program->SetUniform("uModelMatrix", object->Matrix.Matrix());
	}

	void ShadingPassAdapter::Set(Mesh* mesh, Camera* camera) {

	}

}
