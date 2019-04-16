#include <Haze.h>

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace Haze;

class MyAdapter : public ProgramAdapter 
{
	public:
		MyAdapter(Program* p) : ProgramAdapter(p) {}

		virtual void Set(Scene* s, Camera* c) 
		{
			glm::mat4 cam = glm::inverse(glm::lookAt(c->GetWorldPosition(), c->GetWorldPosition() + c->GetDirection(), glm::vec3(0.0f, 1.0f, 0.0f)));
			cam = glm::translate(cam, glm::vec3(0.7f, -1.0f, -2.0f));

			cam = glm::rotate(cam, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			cam = glm::rotate(cam, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			cam = glm::rotate(cam, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			cam = glm::scale(cam, glm::vec3(0.7f));

			_Program->SetUniform("uModelViewProjectionMatrix", c->GetProjectionMatrix() * c->GetViewMatrix() * cam);
			_Program->SetUniform("uColor", 0.0f, 0.0f, 0.0f);
		}

		virtual void Set(Object* o, Camera* c) 
		{

		}	
		
		virtual void Set(Mesh* m, Camera* c)
		{
			_Program->SetUniform("uTexture[0]", m->Textures[0] != nullptr);
			_Program->SetUniform("uTexture[1]", m->Textures[1] != nullptr);
			_Program->SetUniform("uTexture[2]", m->Textures[2] != nullptr);

			_Program->SetUniform("uTexture0", TextureAllocator::Bind(m->Textures[0]));
			_Program->SetUniform("uTexture1", TextureAllocator::Bind(m->Textures[1]));
			_Program->SetUniform("uTexture2", TextureAllocator::Bind(m->Textures[2]));
		}
};

class MyLayer : public Haze::Layer {

	public:

	MyLayer() :
		dr(
			glm::vec4(0.0f, 0.0f, 1280.0f, 720.0f),
			2048, 5,
			new Haze::GeometryPassAdapter(new Haze::Program({
				Haze::Shader::FromFile("../shaders/geom.frag", Haze::ShaderType::Fragment),
				Haze::Shader::FromFile("../shaders/geom.vert", Haze::ShaderType::Vertex)
			})),
			new Haze::ShadingPassAdapter(new Haze::Program({ 
				Haze::Shader::FromFile("../shaders/shade.frag", Haze::ShaderType::Fragment),
				Haze::Shader::FromFile("../shaders/shade.vert", Haze::ShaderType::Vertex),
				Haze::Shader::FromFile("../shaders/shade.geom", Haze::ShaderType::Geometry)
			})),
			new Haze::LightingPassAdapter(new Haze::Program({ 
				Haze::Shader::FromFile("../shaders/light.frag", Haze::ShaderType::Fragment),
				Haze::Shader::FromFile("../shaders/light.vert", Haze::ShaderType::Vertex)
			}))),
		fr(new Haze::ForwardPassAdapter(new Haze::Program({
			Haze::Shader::FromFile("../shaders/default.frag", Haze::ShaderType::Fragment),
			Haze::Shader::FromFile("../shaders/default.vert", Haze::ShaderType::Vertex)
		}))),
		skyb(new Haze::SkyboxAdapter(new Haze::Program({
			Haze::Shader::FromFile("../shaders/skybox.frag", Haze::ShaderType::Fragment),
			Haze::Shader::FromFile("../shaders/skybox.vert", Haze::ShaderType::Vertex)
		}))),
		weap(new MyAdapter(new Program({
			Shader::FromFile("../shaders/default.frag", ShaderType::Fragment),
			Shader::FromFile("../shaders/default.vert", ShaderType::Vertex)
		})))
	{}

	void OnUpdate() override {
		if (Haze::GUI::InFocus()) {
			static float dy = 0, dx = 0;
			static bool jmp = true;

			if (Haze::Input::IsKeyPressed(HZ_KEY_W)) camera.Move(0, 0, 1, true);
			if (Haze::Input::IsKeyPressed(HZ_KEY_S)) camera.Move(0, 0, -1, true);
			if (Haze::Input::IsKeyPressed(HZ_KEY_A)) camera.Move(-1, 0, 0, true);
			if (Haze::Input::IsKeyPressed(HZ_KEY_D)) camera.Move(1, 0, 0, true);

			if (Haze::Input::IsKeyPressed(HZ_KEY_SPACE) && !jmp) {
				jmp = true;
				dy += 1.0f;
			}

			if (jmp) {
				dy -= 0.05f;
			}

			camera.Move(0, dy, 0, true);
			if (camera.GetWorldPosition().y < 2.0f) {
				camera.Set(camera.GetWorldPosition().x, 2.0f, camera.GetWorldPosition().z);
				camera.UpdateMatrices();
				jmp = false;
				dy = 0.0f;
			}

			if (Haze::Input::IsKeyPressed(HZ_KEY_1)) {
				Haze::Model* model = Haze::ModelLoader::Load("C:/users/mar21/desktop/mga/models/sword.obj");
				if (model) {
					Haze::Object* object = new Haze::Object();
					object->Model = model;
					scene.Item = object;
				}
			}
			if (Haze::Input::IsKeyPressed(HZ_KEY_0)) scene.Item = nullptr;

			static bool lookMode = false;
			static bool lookModeLast = false;
			bool lookModeNow = Haze::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_2);

			if (!lookModeLast && lookModeNow) {
				if (lookMode) {
					Haze::Application::Get().GetWindow().SetHideCursor(true);
					lookMode = false;
				} else {
					Haze::Application::Get().GetWindow().SetHideCursor(false);
					lookMode = true;
				}
			}

			lookModeLast = lookModeNow;

			if (lookMode) {
				auto[x, y] = Haze::Input::GetMousePosition();
				static float lastx = x, lasty = y;

				if (lookModeLast) {
					lastx = x;
					lasty = y;
				}

				float xo = x - lastx;
				float yo = lasty - y;

				camera.Look(xo, yo);

				lastx = x;
				lasty = y;
			}
		}
		
		dr.Draw(&scene, &camera);
		
		if (scene.Skybox) 
		{
			skyb.Draw(&scene, &camera, Haze::Mesh::GetCUBE());
		}

		if (scene.Item) 
		{
			weap.Draw(&scene, &camera, scene.Item);
		}
	}

	void OnImGuiRender() override 
	{
		Haze::GUI::Menu(&scene, &camera, dr._DrawMode, dr._DrawNormals);
	}

	void OnEvent(Haze::Event& event) override {
		if (event.GetEventType() == Haze::EventType::WindowResize) {
			Haze::WindowResizeEvent& we = (Haze::WindowResizeEvent&)event;

			camera.OnWindowResizeEvent(we);
			dr.OnWindowResizeEvent(we);
		}
	}

	Haze::DeferredRenderer dr;
	Haze::ForwardRenderer fr, skyb, weap;

	Haze::Scene scene;
	Haze::Camera camera;
};

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new MyLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}