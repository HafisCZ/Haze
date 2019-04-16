#include "hzpch.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace Haze
{

	void Renderer::Draw(Mesh* mesh) {
		mesh->VAO->Bind();
		mesh->IBO->Bind();

		glDrawElements(GL_TRIANGLES, mesh->Triangles.size() * 3, GL_UNSIGNED_INT, nullptr);

		TextureAllocator::Flush();
	}

	void Renderer::Clear() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	DeferredRenderer::DeferredRenderer(glm::vec4 viewport, int shadowResolution, int shadowCount, ProgramAdapter* geometry, ProgramAdapter* shading, ProgramAdapter* lighting) :
		_GBuffer(viewport.z, viewport.w),
		_ShadowMapBufferArray(shadowResolution, shadowCount),
		_GeometryPassAdapter(geometry),
		_ShadingPassAdapter(shading),
		_LightingPassAdapter(lighting)
	{ 

	}

	ForwardRenderer::ForwardRenderer(ProgramAdapter* program) :
		_ForwardPassAdapter(program)
	{

	}

	void ForwardRenderer::Draw(Scene* scene, Camera* camera, Mesh* mesh) 
	{
		_ForwardPassAdapter->Bind();
		_ForwardPassAdapter->Set(scene, camera);
		_ForwardPassAdapter->Set(mesh, camera);

		Renderer::Draw(mesh);

		_ForwardPassAdapter->OnFinish();
	}

	void ForwardRenderer::Draw(Scene* scene, Camera* camera, Object* object) 
	{
		_ForwardPassAdapter->Bind();
		_ForwardPassAdapter->Set(scene, camera);
		_ForwardPassAdapter->Set(object, camera);

		for (auto mesh : object->Model->Meshes) {
			_ForwardPassAdapter->Set(mesh, camera);

			Renderer::Draw(mesh);
		}

		_ForwardPassAdapter->OnFinish();
	}

	void ForwardRenderer::Draw(Scene* scene, Camera* camera) 
	{
		_ForwardPassAdapter->Bind();
		_ForwardPassAdapter->Set(scene, camera);

		for (auto object : scene->Objects) {
			_ForwardPassAdapter->Set(object, camera);

			for (auto mesh : object->Model->Meshes) {
				_ForwardPassAdapter->Set(mesh, camera);

				Renderer::Draw(mesh);
			}
		}

		_ForwardPassAdapter->OnFinish();
	}

	void DeferredRenderer::Draw(Scene* scene, Camera* camera)
	{
		_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_GeometryPassAdapter->Bind();
		_GeometryPassAdapter->Set(scene, camera);

		for (auto object : scene->Objects) 
		{
			_GeometryPassAdapter->Set(object, camera);

			for (auto mesh : object->Model->Meshes) 
			{
				_GeometryPassAdapter->Set(mesh, camera);

				Renderer::Draw(mesh);
			}
		}

		_GBuffer.Unbind();

		bool applyShading = false;
		for (auto& light : scene->Point) 
		{
			if (light.IsCastingShadow()) 
			{
				applyShading = true;
				break;
			}
		}

		if (applyShading) 
		{
			glDisable(GL_CULL_FACE);

			_ShadowMapBufferArray.Bind();

			_ShadingPassAdapter->Bind();
			_ShadingPassAdapter->Set(scene, camera);

			for (auto object : scene->Objects) 
			{
				_ShadingPassAdapter->Set(object, camera);

				for (auto mesh : object->Model->Meshes) 
				{
					Renderer::Draw(mesh);
				}
			}

			_ShadowMapBufferArray.Unbind(3);

			glEnable(GL_CULL_FACE);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_LightingPassAdapter->Bind();

		_GBuffer.BindTextures(0);

		_LightingPassAdapter->Set(scene, camera);
		_LightingPassAdapter->GetProgram()->SetUniform("uDrawMode", _DrawMode);

		Renderer::Draw(Mesh::GetPLANE());
		
		_GBuffer.Copy();

		if (_DrawNormals > 0)
		{
			static Program p({
				Shader::FromFile("../shaders/nvisual.vert", ShaderType::Vertex),
				Shader::FromFile("../shaders/nvisual.frag", ShaderType::Fragment),
				Shader::FromFile("../shaders/nvisual.geom", ShaderType::Geometry)
			});

			p.Bind();
			p.SetUniform("uVertexNormalColor", 244.0f / 255.0f, 235.0f / 255.0f, 66.0f / 255.0f, 1.0f);
			p.SetUniform("uFaceNormalColor", 66.0f / 255.0f, 134.0f / 255.0f, 244.0f / 255.0f, 1.0f);
			p.SetUniform("uVertexNormalE", _DrawNormals & BIT(0));
			p.SetUniform("uFaceNormalE", _DrawNormals & BIT(1));
			p.SetUniform("uNormalLength", 0.25f);

			for (auto obj : scene->Objects) {
				p.SetUniform("uModelViewProjectionMatrix", camera->GetProjectionMatrix() * camera->GetViewMatrix() * obj->Matrix.Matrix());
				for (auto mesh : obj->Model->Meshes) {
					Renderer::Draw(mesh);
				}
			}
		}
	}

	void DeferredRenderer::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		_GBuffer.Resize(event.GetWidth(), event.GetHeight());
	}

}
