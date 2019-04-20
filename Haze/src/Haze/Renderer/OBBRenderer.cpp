#include "hzpch.h"
#include "OBBRenderer.h"

#include "Haze/Program/Program.h"

#include <glad/glad.h>

namespace Haze {

	void OBBRenderer::Draw(Scene* scene, Camera* camera, Object* so, Mesh* sm) {
		static Program OBBProgram({
			Shader::FromFile("shaders/obb.vert", ShaderType::Vertex),
			Shader::FromFile("shaders/obb.frag", ShaderType::Fragment),
			Shader::FromFile("shaders/obb.geom", ShaderType::Geometry)
		});

		static unsigned int VAO = []() {
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			return VAO;
		}();
		static unsigned int VBO = []() {
			glm::vec3 position = { 0.0f, 0.0f, 0.0f };
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &position.x, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
			return VBO;
		}();

		glBindVertexArray(VAO);
		OBBProgram.Bind();

		for (auto obj : scene->Objects) {
			OBBProgram.SetUniform("uModelViewProjectionMatrix", camera->GetProjectionMatrix() * camera->GetViewMatrix() * obj->Matrix.Matrix());

			for (auto mesh : obj->Model->Meshes) {
				if (obj == so) {
					if (mesh == sm) {
						OBBProgram.SetUniform("uColor", glm::vec3(RGB3TOF(173,255,47)));
					} else {
						OBBProgram.SetUniform("uColor", glm::vec3(RGB3TOF(20, 100, 15)));
					}
				} else {
					OBBProgram.SetUniform("uColor", glm::vec3(0.2f));
				}

				OBBProgram.SetUniform("uMin", mesh->AABB_MIN);
				OBBProgram.SetUniform("uMax", mesh->AABB_MAX);

				glDrawArrays(GL_POINTS, 0, 1);
			}
		}
	}

}