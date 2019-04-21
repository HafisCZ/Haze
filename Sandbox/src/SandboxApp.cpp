#include <Haze.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace Haze;

class KeyLock {
	private:
		bool _Value = false;
		bool _State = false;
		bool _Last = false;

	public:
		bool operator()() {
			return _State;
		}

		bool operator()(bool value) {
			bool result = value && !_Value;
			_Value = value;		
			_Last = result;
			if (result) _State = !_State;
			return result;
		}

		bool last() {
			return _Last;
		}
};

class RenderLayer : public Layer {
	private:
		DeferredRenderer dr;
		OBBRenderer or ;
		ForwardRenderer sk;

		Camera camera;
		Scene scene;

		std::vector<std::pair<Object*, Mesh*>> s;
		int si = -1;

		Object* so = nullptr;
		Mesh* sm = nullptr;

	public:
		RenderLayer() : 
			Layer("SandboxRenderLayer"), 
			dr({ 0.0f, 0.0f, 1280.0f, 720.0f }, 2048, 5), 
			or(), 
			sk(new SkyboxAdapter(new Program({Shader::FromFile("shaders/skybox.frag", ShaderType::Fragment), Shader::FromFile("shaders/skybox.vert", ShaderType::Vertex)})))
		{}

		void InputUpdate() {
			static KeyLock KP_M1;
			static KeyLock KP_M2;
			static KeyLock KP_M3;
			static KeyLock KP_Q;

			if (KP_M1(Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_1)) && KP_M3()) {
				if (s.size() > 0) {
					if (si == -1) si = 0;
					else si = ++si % s.size();
				} else {
					si = -1;
				}
			}

			KP_M2(Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_2));
			
			if (KP_M3(Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_3))) {
				Haze::Application::Get().GetWindow().SetHideCursor(!KP_M3());
			}
			
			KP_Q(Input::IsKeyPressed(HZ_KEY_Q));

			static float yDelta = 0.0f;
			static bool yInAir = false;

			if (KP_M3()) {
				auto[x, y] = Input::GetMousePosition();
				static float lx = x, ly = y;
				if (KP_M3.last()) { lx = x; ly = y; }
				float xo = x - lx, yo = ly - y;
				camera.Look(xo, yo);
				lx = x; ly = y;

				if (!KP_Q()) {
					if (Input::IsKeyPressed(HZ_KEY_W)) camera.Move(0, 0, +1, true);
					if (Input::IsKeyPressed(HZ_KEY_S)) camera.Move(0, 0, -1, true);
					if (Input::IsKeyPressed(HZ_KEY_D)) camera.Move(+1, 0, 0, true);
					if (Input::IsKeyPressed(HZ_KEY_A)) camera.Move(-1, 0, 0, true);
					if (Input::IsKeyPressed(HZ_KEY_SPACE) && !yInAir) {
						yInAir = true;
						yDelta = 1.0f;
					}
				} else {
					if (Input::IsKeyPressed(HZ_KEY_W)) camera.Move(0, 0, +1);
					if (Input::IsKeyPressed(HZ_KEY_S)) camera.Move(0, 0, -1);
					if (Input::IsKeyPressed(HZ_KEY_D)) camera.Move(+1, 0, 0);
					if (Input::IsKeyPressed(HZ_KEY_A)) camera.Move(-1, 0, 0);
					if (Input::IsKeyPressed(HZ_KEY_SPACE)) camera.Move(0, +1, 0);
					if (Input::IsKeyPressed(HZ_KEY_C)) camera.Move(0, -1, 0);
				}
			}
			
			if (!KP_Q()) {
				if (yInAir) {
					yDelta -= 0.05f;
					camera.Move(0.0, yDelta, 0.0, true);
				}

				glm::vec3 pos = camera.GetWorldPosition();

				if (pos.y > 2.0f) {
					yInAir = true;
				} else if (pos.y < 2.0f) {
					yInAir = false;
					camera.Set(pos.x, 2.0f, pos.z);
				}
			}
		}

		void SceneUpdate() {
			s.clear();

			for (auto o : scene.Objects) {
				auto inr = o->IntersectsRay(camera.GetWorldPosition(), glm::normalize(camera.GetDirection()));
				if (inr.first) {
					s.push_back({ o, inr.first });
				}
			}

			if (si == -1 || si >= s.size()) {
				si = -1;
				so = nullptr;
				sm = nullptr;
			} else {
				so = s[si].first;
				sm = s[si].second;
			}
		}

		void Draw() {
			dr.Draw(&scene, &camera);
			or.Draw(&scene, &camera, so, sm);

			if (scene.Skybox) sk.Draw(&scene, &camera);
		}

		virtual void OnUpdate() override {
			if (GUI::InFocus()) {
				InputUpdate();
			}

			SceneUpdate();
			Draw();
		}

		virtual void OnImGuiRender() override {
			Haze::UI::ShowUI(&scene, &camera, so, sm);

			/*
				will be used until new UI is implemented properly
			*/
			Haze::GUI::Menu(&scene, &camera, dr._DrawMode, dr._DrawOverlayMode);
		}

		void OnEvent(Haze::Event& e) override {
			if (e.GetEventType() == Haze::EventType::WindowResize) {
				Haze::WindowResizeEvent& we = (Haze::WindowResizeEvent&)e;

				camera.OnWindowResizeEvent(we);
				dr.OnWindowResizeEvent(we);
			}
		}
};

class Sandbox : public Haze::Application
{
	public:
		Sandbox() 
		{
			PushLayer(new RenderLayer());
		}

		~Sandbox() 
		{

		}
};

Haze::Application* Haze::CreateApplication()
{
	return new Sandbox();
}