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

	DeferredRenderer::DeferredRenderer(glm::vec4 viewport, int shadowResolution, int shadowCount) :
		_GBuffer((unsigned int) viewport.z, (unsigned int) viewport.w),
		_ShadowMapBufferArray(shadowResolution, shadowCount),
		_GeometryPassAdapter(new GeometryPassAdapter(new Program({
			Shader::FromFile("shaders/geom.frag", ShaderType::Fragment),
			Shader::FromFile("shaders/geom.vert", ShaderType::Vertex)
		}))),
		_ShadingPassAdapter(new ShadingPassAdapter(new Program({
			Shader::FromFile("shaders/shade.frag", ShaderType::Fragment),
			Shader::FromFile("shaders/shade.vert", ShaderType::Vertex),
			Shader::FromFile("shaders/shade.geom", ShaderType::Geometry)
		}))),
		_LightingPassAdapter(new LightingPassAdapter(new Program({
			Shader::FromFile("shaders/light.frag", ShaderType::Fragment),
			Shader::FromFile("shaders/light.vert", ShaderType::Vertex)
		})))
	{

	}

	ForwardRenderer::ForwardRenderer(ProgramAdapter* program) :
		_ForwardPassAdapter(program) {

	}

	void ForwardRenderer::Draw(Scene* scene, Camera* camera, Mesh* mesh) {
		_ForwardPassAdapter->Bind();
		_ForwardPassAdapter->Set(scene, camera);
		_ForwardPassAdapter->Set(mesh, camera);

		Renderer::Draw(mesh);

		_ForwardPassAdapter->OnFinish();
	}

	void ForwardRenderer::Draw(Scene* scene, Camera* camera, Object* object) {
		_ForwardPassAdapter->Bind();
		_ForwardPassAdapter->Set(scene, camera);
		_ForwardPassAdapter->Set(object, camera);

		for (auto mesh : object->Model->Meshes) {
			_ForwardPassAdapter->Set(mesh, camera);

			Renderer::Draw(mesh);
		}

		_ForwardPassAdapter->OnFinish();
	}

	void ForwardRenderer::Draw(Scene* scene, Camera* camera) {
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
		if (_DrawOverlayMode & BIT(2)) glDisable(GL_CULL_FACE);

		_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_GeometryPassAdapter->Bind();
		_GeometryPassAdapter->Set(scene, camera);

		unsigned int stencilIndex = 1;

		for (auto object : scene->Objects) 
		{
			_GeometryPassAdapter->Set(object, camera);

			glStencilFunc(GL_ALWAYS, stencilIndex++, 0xFF);

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

		if (_DrawOverlayMode > 0)
		{
			static Program p({
				Shader::FromFile("../shaders/nvisual.vert", ShaderType::Vertex),
				Shader::FromFile("../shaders/nvisual.frag", ShaderType::Fragment),
				Shader::FromFile("../shaders/nvisual.geom", ShaderType::Geometry)
			});

			p.Bind();
			p.SetUniform("uVertexNormalColor", 244.0f / 255.0f, 235.0f / 255.0f, 66.0f / 255.0f, 1.0f);
			p.SetUniform("uFaceNormalColor", 66.0f / 255.0f, 134.0f / 255.0f, 244.0f / 255.0f, 1.0f);
			p.SetUniform("uVertexNormalE", _DrawOverlayMode & BIT(0));
			p.SetUniform("uFaceNormalE", _DrawOverlayMode & BIT(1));
			p.SetUniform("uNormalLength", 0.25f);

			for (auto obj : scene->Objects) {
				p.SetUniform("uModelViewProjectionMatrix", camera->GetProjectionMatrix() * camera->GetViewMatrix() * obj->Matrix.Matrix());
				for (auto mesh : obj->Model->Meshes) {
					Renderer::Draw(mesh);
				}
			}
		}

		if (_DrawOverlayMode & BIT(2)) glEnable(GL_CULL_FACE);

		//glReadPixels(_Viewport[0] / 2, _Viewport[1] / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &_Sample.first);
		//glReadPixels(_Viewport[0] / 2, _Viewport[1] / 2, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &_Sample.second);
	}

	void DeferredRenderer::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		_GBuffer.Resize(event.GetWidth(), event.GetHeight());
	}

}
