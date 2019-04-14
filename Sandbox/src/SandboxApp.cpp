#include <Haze.h>

#include "imgui/imgui.h"

class MyLayer : public Haze::Layer {

	public:

	MyLayer() :
		dr(
			glm::vec4(0.0f, 0.0f, 1280.0f, 720.0f),
			2048, 5,
			new Haze::GeometryPassAdapter(new Haze::Program("../shaders/geom", Haze::ShaderTypeFragment | Haze::ShaderTypeVertex)),
			new Haze::ShadingPassAdapter(new Haze::Program("../shaders/shade", Haze::ShaderTypeGeometry | Haze::ShaderTypeFragment | Haze::ShaderTypeVertex)),
			new Haze::LightingPassAdapter(new Haze::Program("../shaders/light", Haze::ShaderTypeFragment | Haze::ShaderTypeVertex))
		),
		fr(new Haze::ForwardPassAdapter(new Haze::Program("../shaders/default", Haze::ShaderTypeFragment | Haze::ShaderTypeVertex))),
		skyb(new Haze::SkyboxAdapter(new Haze::Program("../shaders/skybox", Haze::ShaderTypeFragment | Haze::ShaderTypeVertex)))
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
	}

	void OnImGuiRender() override 
	{
		Haze::GUI::Menu(&scene, &camera, dr._DrawMode);
	}

	void OnEvent(Haze::Event& event) override {
		if (event.GetEventType() == Haze::EventType::WindowResize) {
			Haze::WindowResizeEvent& we = (Haze::WindowResizeEvent&)event;

			camera.OnWindowResizeEvent(we);
			dr.OnWindowResizeEvent(we);
		}
	}

	Haze::DeferredRenderer dr;
	Haze::ForwardRenderer fr, skyb;

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